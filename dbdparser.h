#ifndef DBDPARSER_H
#define DBDPARSER_H

#include <vector>
#include <list>

#include <boost/shared_ptr.hpp>

#include "dbdlexer.h"

struct Node
{
    typedef boost::shared_ptr<Node> pointer;
    unsigned line, col;
    Node() :line(0),col(0) {}
};

struct Command : public Node
{
    std::string name, val;
};

struct Code : public Node
{
    std::string val;
};

struct Comment : public Node
{
    std::string val;
};

struct Block : public Node
{
    std::string name;
    std::vector<std::string> args;
    std::list<Node::pointer> children;
};

typedef std::list<Node::pointer> Top;

class DBDParser : public DBDLexer
{
public:
    DBDParser();

    Top tree;
    virtual void reset();

    enum parState_t {
        parDBD, parCoB, parCom, parCode, parArg, parArgCont, parTail
    } parState;
    static const char* parStateName(parState_t S);

    bool parDebug;
private:
    virtual void token();

    //std::vector<boost::shared_ptr<Block> > stack;
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

    virtual void parse_eoi();

    DBDToken CoBtoken;
    std::vector<std::string> blockargs;
};

#endif // DBDPARSER_H
