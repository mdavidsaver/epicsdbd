#include <stdexcept>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "dbdlexer.h"

DBDLexer::DBDLexer()
{ reset(); }

void DBDLexer::token() {}

void DBDLexer::reset()
{
    tokState = tokInit;
    tok.reset();
}

std::ostream& operator<<(std::ostream& strm, const DBDToken& t)
{
    strm<<t.line<<":"<<t.col<<"\t"<<t.value;
    return strm;
}

const char* DBDLexer::tokStateName(tokState_t S)
{
    switch(S) {
#define STATE(S) case DBDLexer::S: return #S
    STATE(tokInit);
    STATE(tokLit);
    STATE(tokWS);
    STATE(tokQuote);
    STATE(tokEsc);
    STATE(tokBare);
    STATE(tokCode);
    STATE(tokComment);
    STATE(tokEOI);
#undef STATE
    default:
        return "<invalid>";
    }
}

static
std::string LexError(const DBDLexer& L, const char *msg)
{
    std::ostringstream strm;
    strm<<"Lexer error at "<<L.tok.line<<":"<<L.tok.col<<" : "<<msg;
    return strm.str();
}
#define THROW(msg) throw std::runtime_error(LexError(*this, msg))

static
std::string InvalidChar(const DBDLexer& L, char c)
{
    std::ostringstream strm;
    strm<<"Invalid charactor at "<<L.tok.line<<":"<<L.tok.col
        <<" : '"<<c<<"' ("<<int(c)<<") State "
        <<DBDLexer::tokStateName(L.tokState);
    return strm.str();
}
#define INVALID(c) throw std::runtime_error(InvalidChar(*this, c))

static bool iswordchar(char c)
{
    if(c>='a' && c<='z') return true;
    else if(c>='A' && c<='Z') return true;
    else if(c>='0' && c<='9') return true;
    else switch(c) {
    case '_':
    case '-':
    case '+':
    case ':':
    case ';':
    case '.':
    case '[':
    case ']':
    case '<':
    case '>': return true;
    default: return false;
    }
}

void DBDLexer::lex(std::istream &strm)
{
    char c;
    while(strm.get(c)) {
        if(c=='\n')
            tok.newline();
        else
            tok.inc();

//        std::cerr<<line<<":"<<col<<" in "<<DBDLexer::tokStateName(tokState)
//                 <<" '"<<c<<"' ("<<int(c)<<")\n";

        switch(tokState) {
        case tokInit:
            assert(tok.value.empty());
            /*
            tokInit : '"' -> tokQuote
                    | [a-zA-Z0-9_\-+:.\[\]<>;] -> yymore -> tokBare
                    | '%' -> tokCode
                    | '#' -> tokComment
                    | [ \t\n\r] -> tokWS
                    | [(){},] -> tok(literal) -> tokInit
                    | EOI -> done
                    | . -> error
             */
            switch(c) {
            case '"': tokState = tokQuote; break;
            case '%': tokState = tokCode; break;
            case '#': tokState = tokComment; break;
            case '(':
            case ')':
            case '{':
            case '}':
            case ',': tokState = tokLit;
                tok.push_back(c);
                token();
                tok.reset();
                tokState = tokInit;
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\n': tokState = tokInit; break;
            default:
                if(iswordchar(c)){
                    tokState = tokBare;
                    tok.push_back(c);
                } else {
                    INVALID(c);
                }
            }
            break;

        case tokWS:
            assert(tok.value.empty());
            /*
            tokWS : [ \t\n\r] -> tokInit
                  | [(){},] -> tok(literal) -> tokInit
                  | EOI -> done
                  | . -> error
             */
            switch(c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n': tokState = tokWS; break;
            case '(':
            case ')':
            case '{':
            case '}':
            case ',':
                tokState = tokLit;
                tok.push_back(c);
                token();
                tok.reset();
                tokState = tokInit;
                break;
            default:
                INVALID(c);
            }

            break;

        case tokQuote:
            /*
            tokQuote : '\\' -> tokEsc
                     | [\n\r] -> error
                     | '"' -> tok(quote) -> tokWS
                     | . -> yymore -> tokQuote
                     | EOI -> error
             */
            switch(c) {
            case '\\': tokState = tokEsc; break;
            case '\n':
            case '\r':
                THROW("Missing closing quote");
            case '"':
                token();
                tok.reset();
                tokState = tokWS;
                break;
            default:
                tok.push_back(c);
                tokState = tokQuote;
            }
            break;

        case tokEsc:
            /*
            st_esc : . -> yymore -> st_quote
                   | EOI -> error
             */
            tok.push_back(c);
            tokState = tokQuote;
            break;

        case tokBare:
            /*
            tokBare : [a-zA-Z0-9_\-+:.\[\]<>;] -> yymore -> tokBare
                    | [ \t\n\r] -> tok(bareword) -> tokWS
                    | [(){},] -> tok(literal) -> tokInit
                    | EOI -> done
                    | . -> error
             */
            if(iswordchar(c))
                tok.push_back(c);
            else switch(c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                token();
                tok.reset();
                tokState = tokWS; break;
            case '(':
            case ')':
            case '{':
            case '}':
            case ',':
                token();
                tok.reset();
                tokState = tokLit;
                tok.push_back(c);
                token();
                tok.reset();
                tokState = tokInit;
                break;
            default:
                INVALID(c);
            }

            break;

        case tokCode:
        case tokComment:
            /*
            tokCode : [\r\n]EOI -> tok(code) -> tokInit
                    | . -> yymore -> tokCode
            tokComment : [\r\n]EOI -> tok(comment) -> tokInit
                       | . -> yymore -> tokComment
             */
            switch(c) {
            case '\r':
            case '\n':
                token();
                tok.reset();
                tokState = tokInit;
                break;
            default:
                tok.push_back(c);
            }

            break;

        default:
            THROW("Invalid state");
        }
    }

    // EOI
    switch(tokState) {
    case tokInit:
    case tokWS:
    case tokCode:
    case tokComment:
        token();
        break;
    default:
        THROW("Unexpected end of input");
    }
    tokState = tokEOI;
    token();
}
