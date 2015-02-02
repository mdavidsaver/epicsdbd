#ifndef DBDPARSER_H
#define DBDPARSER_H

#include <vector>
#include <list>

#include "dbdlexer.h"

/** @brief Parser for DB/DBD grammar
 *
 @code
    value : tokBare | tokQuote

    entry : command
          | code
          | comment
          | block

    command : value value

    code : tokCode
    comment : tokComment

    block : Bareword block_head
          | Bareword block_head bock_body

    block_head : '(' ')'
               | '(' value *(',' value) ')'

    block_body : '{' dbd '}'

    dbd :
        | entry
        | entry dbd
 @endcode
 */
class DBDParser : public DBDLexer
{
public:
    DBDParser();
    virtual ~DBDParser(){};

    virtual void reset();

    enum parState_t {
        parDBD, parCoB, parCom, parCode, parArg, parArgCont, parTail
    } parState;
    static const char* parStateName(parState_t S);

    virtual void lex(std::istream&);

    bool parDebug;
private:
    virtual void token();

protected:
    unsigned depth;

    //! Command name from CoBtoken and arg from tok
    virtual void parse_command()=0;

    //! comment from tok
    virtual void parse_comment()=0;
    //! code from tok
    virtual void parse_code()=0;

    //! Command name from CoBtoken and args from blockargs
    virtual void parse_block()=0;
    //! Mark start of block body
    virtual void parse_block_body_start()=0;
    //! Mark start of block body
    virtual void parse_block_body_end()=0;

    virtual void parse_start();
    virtual void parse_eoi();

    DBDToken CoBtoken;
    std::vector<std::string> blockargs;
};

#endif // DBDPARSER_H
