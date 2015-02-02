
#include <stdexcept>

#include "dbdparser.h"

DBDParser::DBDParser()
    :parState(parDBD)
{}

void DBDParser::reset()
{
    tree.clear();
    DBDLexer::reset();
}

void DBDParser::token()
{
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
            if(stack.size()==1) {
                tree.swap(stack.front().children);
                return;
            } else
                throw std::runtime_error("EOI before }");
            break;

        case tokBare:
            // shift token
            parState = parCoB;
            break;

        case tokLit:
            assert(tok.size()==1);
            switch(tok.at(0))
            {
            case '#':
                // reduce comment
                parState = parDBD; break;
            case '%':
                // reduce code
                parState = parDBD;break;
            case '}':
                // reduce block
                if(stack)
                parState = parDBD;
                break;
            case '{':
                if(parState==parTail) {
                    // reduce initbody
                    parState = parDBD;
                    break;
                }
            default:
                throw std::runtime_error("Unexpected literal");
            }
            break;

        default:
            throw std::runtime_error("Unexpected token");
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
            parState = parDBD;
            break;

        case tokLit:
            assert(tok.size()==1);
            if(tok.at(0)=='(') {
                // reduce initblock
                parState = parArg;
            } else
                throw std::runtime_error("Unexpected literal");
            break;

        default:
            throw std::runtime_error("Unexpected token");
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
            parState = parArgCont;
            break;

        case tokLit:
            assert(tok.size()==1);
            if(tok.at(0)==')') {
                // reduce blockargs
                parState = parTail;
            } else
                throw std::runtime_error("Unexpected literal");
            break;

        default:
            throw std::runtime_error("Unexpected token");

        }
        break;

    case parArgCont:
        /*
        state_arg_cont : ',' -> state_arg
                       | ')' -> reduce(blockargs) -> state_tail
                       | . -> error
         */
        switch(tokState) {
        case tokLit:
            assert(tok.size()==1);
            switch(tok.at(0))
            {
            case ',': parState = parArg; break;
            case ')':
                // reduce blockargs
                parState = parTail;
                break;
            default:
                throw std::runtime_error("Unexpected literal");
            }
            break;

        default:
            throw std::runtime_error("Unexpected token");

        }
        break;

    default:
        throw std::logic_error("Invalid parser state");
    }
}
