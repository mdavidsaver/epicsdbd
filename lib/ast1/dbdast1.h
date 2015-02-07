#ifndef DBDAST_H
#define DBDAST_H

#include <vector>
#include <list>

#include <boost/shared_ptr.hpp>

#include "dbdparser.h"

struct Node
{
    typedef boost::shared_ptr<Node> pointer;
    enum Type {
        cmd=1, code, comment, block
    } ntype;

    unsigned line, col;
    Node(Type t) :ntype(t), line(0),col(0) {}
};

struct Command : public Node
{
    std::string name, value;
    Command() : Node(cmd) {}
};

struct Code : public Node
{
    std::string value;
    Code() : Node(code) {}
};

struct Comment : public Node
{
    std::string value;
    Comment() : Node(comment) {}
};

struct Block : public Node
{
    std::string name;
    typedef std::vector<std::string> args_t;
    args_t args;
    typedef std::list<Node::pointer> children_t;
    children_t children;
    Block() : Node(block) {}
};

typedef Block::children_t Top;

class DBDASTParser : public DBDParser
{
public:
    DBDASTParser();

    virtual void reset();

    Top tree;

private:
    virtual void parse_command(DBDToken& cmd, DBDToken& arg);
    virtual void parse_comment(DBDToken&);
    virtual void parse_code(DBDToken&);
    virtual void parse_block(DBDToken&, blockarg_t&);
    virtual void parse_block_body_start();
    virtual void parse_block_body_end();
    virtual void parse_start();
    virtual void parse_eoi();

    Block tempblock;
    std::vector<boost::shared_ptr<Block> > stack;
};

/** @brief Render DB/DBD tree
 *
 * Uses quoting rules of Base
 */
std::ostream& DBDASTPrinter(std::ostream&, const Top&, unsigned indent=0);

#endif // DBDAST_H
