#include <stdexcept>
#include <sstream>

#include "dbdlexer.h"

DBDLexer::DBDLexer()
{ reset(); }

void DBDLexer::token() {}

void DBDLexer::reset()
{
    tokState = tokInit;
    tok.clear();
    line = 1;
    col = 0;
}

static
std::string LexError(const DBDLexer& L, const char *msg)
{
    std::ostringstream strm;
    strm<<"Lexer error at "<<L.line<<":"<<L.col<<" : "<<msg;
    return strm.str();
}
#define THROW(msg) throw std::runtime_error(LexError(*this, msg))

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
        if(c=='\n') {
            line++;
            col=0;
        } else
            col++;

        switch(tokState) {
        case tokInit:
            /*
            tokInit : '"' -> tokQuote
                    | [a-zA-Z0-9_\-+:.\[\]<>;] -> yymore -> tokBare
                    | '%' -> tokCode
                    | '#' -> tokComment
                    | [ \t\n\r] -> tokWS
                    | [(){},] -> tok(literal) -> st_init
                    | EOI -> st_done
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
                tok.clear();
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
                    THROW("Invalid charactor");
                }
            }
            break;

        case tokWS:
            /*
            st_ws : [ \t\n\r] -> st_init
                  | EOI -> st_done
                  | . -> error
             */
            switch(c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n': tokState = tokWS; break;
            default:
                THROW("Invalid charactor");
            }

            break;

        case tokQuote:
            /*
            tokQuote : '\\' -> tokEsc
                     | [\n\r] -> error
                     | '"' -> tok(quote) -> tokWS
                     | [(){},] -> tok(literal) -> tokInit
                     | . -> yymore -> tokQuote
                     | EOI -> error
             */
            switch(c) {
            case '\\': tokState = tokEsc; break;
            case '\n':
            case '\r':
                THROW("Invalid charactor");
            case '"':
                token();
                tok.clear();
                tokState = tokWS;
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
                tok.clear();
                tokState = tokWS; break;
            default:
                THROW("Invalid charactor");
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
                tok.clear();
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
}
