
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>

#include "dbdparser.h"

DBDParser::DBDParser()
    :parState(parDBD)
    ,parDebug(false)
    ,depth(0)
{}

void DBDParser::reset()
{
    parState = parDBD;
    depth = 0;
    CoBtoken.reset();
    blockargs.clear();
    DBDLexer::reset();
}

void DBDParser::lex(std::istream& s)
{
    parse_start();
    DBDLexer::lex(s);
}

void DBDParser::parse_command(){}
void DBDParser::parse_comment(){}
void DBDParser::parse_code(){}
void DBDParser::parse_block(){}
void DBDParser::parse_block_body_start(){}
void DBDParser::parse_block_body_end(){}
void DBDParser::parse_start(){}
void DBDParser::parse_eoi(){}

const char* DBDParser::parStateName(parState_t S)
{
    switch(S) {
#define STATE(s) case s: return #s;
    STATE(parDBD);
    STATE(parCoB);
    STATE(parCom);
    STATE(parCode);
    STATE(parArg);
    STATE(parArgCont);
    STATE(parTail);
#undef STATE
    default:
        return "<invalid>";
    }
}

static void invalidToken(const DBDParser& t)
{
    std::ostringstream strm;
    strm<<"Invalid token: "<<DBDParser::tokStateName(t.tokState)<<" "<<t.tok;
    throw std::runtime_error(strm.str());
}

void DBDParser::token()
{
    if(parDebug)
        std::cerr<<"Parse "<<depth
                 <<" State: "<<parStateName(parState)
                 <<" Tok: "<<tokStateName(tokState)<<" "
                 <<tok<<"\n";

    switch(parState)
    {
    case parDBD:
    case parTail:
        /*
        state_dbd : EOI -> reduce(finish) -> state_done
                  | Bareword -> shift -> state_CoB
                  | '#' -> reduce(comment) -> state_dbd
                  | '%' -> reduce(code) -> state_dbd
                  | '}' -> reduce(block) -> state_dbd
                  | . -> error

        state_tail : '{' -> reduce(initbody) -> state_dbd
                   | EOI -> reduce(finish) -> state_done
                   | Bareword -> shift -> state_CoB
                   | '#' -> reduce(comment) -> state_dbd
                   | '%' -> reduce(code) -> state_dbd
                   | '}' -> reduce(block) -> state_dbd
                   | . -> error
         */
        switch(tokState) {
        case tokEOI:
            if(depth==0) {
                parse_eoi();
                return;
            } else
                throw std::runtime_error("EOI before }");
            break;

        case tokBare:
            // shift token
            CoBtoken.take(tok);
            parState = parCoB;
            break;

        case tokLit:
            assert(tok.size()==1);
            switch(tok.value.at(0))
            {
            case '#':
                // reduce comment
                parse_comment();
                parState = parDBD; break;
            case '%':
                // reduce code
                parse_code();
                parState = parDBD;break;
            case '}':
                // reduce block
                if(depth==0)
                    throw std::runtime_error("'}' without '{'");
                depth--;
                parse_block_body_end();
                parState = parDBD;
                break;
            case '{':
                if(parState==parTail) {
                    // reduce initbody
                    if(depth==0xffff)
                        throw std::runtime_error("Block depth limit exceed");
                    parse_block_body_start();
                    depth++;
                    parState = parDBD;
                    break;
                }
            default:
                throw std::runtime_error("Unexpected literal");
            }
            break;

        default:
            invalidToken(*this);
        }
        break;

    case parCoB:
        /*
        state_CoB : Bareword -> shift -> reduce(command) -> state_dbd
                  | Quoted -> shift -> reduce(command) -> state_dbd
                  | '(' -> reduce(initblock) -> state_arg
                  | . -> error
         */
        switch(tokState)
        {
        case tokBare:
        case tokQuote:
            // reduce command
            parse_command();
            CoBtoken.reset();
            parState = parDBD;
            break;

        case tokLit:
            assert(tok.size()==1);
            if(tok.value.at(0)=='(') {
                // reduce initblock
                blockargs.clear();
                parState = parArg;
            } else
                throw std::runtime_error("Unexpected literal");
            break;

        default:
            invalidToken(*this);
        }
        break;

    case parArg:
        /*
        state_arg : ')' -> reduce(blockargs) -> state_tail
                  | Bareword -> shift -> state_arg_cont
                  | Quoted -> shift -> state_arg_cont
                  | . -> error
         */
        switch(tokState) {
        case tokBare:
        case tokQuote:
            // shift arg
            blockargs.push_back(tok.value); //TODO: is a copy?
            parState = parArgCont;
            break;

        case tokLit:
            assert(tok.size()==1);
            if(tok.value.at(0)==')') {
                // reduce blockargs
                parse_block();
                parState = parTail;
            } else
                throw std::runtime_error("Unexpected literal");
            break;

        default:
            invalidToken(*this);

        }
        break;

    case parArgCont:
        /*
        state_arg_cont : ',' -> state_arg
                       | ')' -> reduce(blockargs) -> state_tail
                       | . -> error
         */
        if(tokState!=tokLit)
            invalidToken(*this);

        assert(tok.size()==1);
        switch(tok.value.at(0))
        {
        case ',': parState = parArg; break;
        case ')':
            // reduce blockargs
            parse_block();
            parState = parTail;
            break;
        default:
            throw std::runtime_error("Unexpected literal");
        }
        break;

    default:
        throw std::logic_error("Invalid parser state");
    }
}
