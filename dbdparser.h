#ifndef DBDPARSER_H
#define DBDPARSER_H

#include <vector>

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

protected:
    virtual void token();

    std::vector<boost::shared_ptr<Block> > stack;

};

#endif // DBDPARSER_H
