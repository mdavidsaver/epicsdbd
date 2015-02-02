
#include "dbdast.h"
#include "util.h"

DBDASTParser::DBDASTParser()
{}

void DBDASTParser::reset()
{
    DBDParser::reset();
    tree.clear();
    stack.clear();
}


void DBDASTParser::parse_command()
{
    assert(stack.size()==depth+1);
    boost::shared_ptr<Command> P(new Command());
    P->line = CoBtoken.line;
    P->col = CoBtoken.col;
    P->name.swap(CoBtoken.value);
    stack.back()->children.push_back(P);
}

void DBDASTParser::parse_comment()
{
    assert(stack.size()==depth+1);
    boost::shared_ptr<Comment> P(new Comment());
    P->line = tok.line;
    P->col = tok.col;
    P->value.swap(tok.value);
    stack.back()->children.push_back(P);
}

void DBDASTParser::parse_code()
{
    assert(stack.size()==depth+1);
    boost::shared_ptr<Code> P(new Code());
    P->line = tok.line;
    P->col = tok.col;
    P->value.swap(tok.value);
    stack.back()->children.push_back(P);
}

void DBDASTParser::parse_block()
{
    assert(stack.size()==depth+1);
    boost::shared_ptr<Block> P(new Block());
    P->line = CoBtoken.line;
    P->col = CoBtoken.col;
    P->name.swap(CoBtoken.value);
    P->args.swap(blockargs);
    stack.back()->children.push_back(P);
}

void DBDASTParser::parse_block_body_start()
{
    assert(stack.size()==depth+1);
    assert(stack.back()->children.size()>0);
    boost::shared_ptr<Block> P(boost::shared_static_cast<Block>(stack.back()->children.back()));
    stack.push_back(P);
}

void DBDASTParser::parse_block_body_end()
{
    stack.pop_back();
    assert(stack.size()==depth+1);
}

void DBDASTParser::parse_start()
{
    stack.clear();
    // pseudo block
    boost::shared_ptr<Block> B(new Block());
    stack.push_back(B);
}

void DBDASTParser::parse_eoi()
{
    assert(stack.size()==1);
    tree.swap(stack.back()->children);
}

static
unsigned quoteArg(const std::string& name)
{
    if(name=="menu" || name=="recordtype" || name=="field"
            || name=="special" || name=="size" || name=="promptgroup"
            || name=="interest" || name=="prop" || name=="pp"
            || name=="asl" || name=="base" || name=="registrar"
            || name=="variable")
        return 999;
    else if(name=="choice")
        return 1;
    else if(name=="device")
        return 3;
    return 0;
}

std::ostream& DBDASTPrinter(std::ostream& strm, const Top& t, unsigned indent)
{
    std::string id(indent,'\t');
    Top::const_iterator it, end;
    for(it=t.begin(), end=t.end(); it!=end; ++it)
    {
        const Node *N = (*it).get();
        switch(N->ntype) {
        case Node::cmd: {
            const Command *C=static_cast<const Command*>(N);
            strm<<id<<C->name;
            streamEscape(strm, C->value);
            strm<<")\n";
        } break;
        case Node::code: {
            const Code *C=static_cast<const Code*>(N);
            strm<<id<<"%"<<C->value<<"\n";
        } break;
        case Node::comment: {
            const Comment *C=static_cast<const Comment*>(N);
            strm<<id<<"%"<<C->value<<"\n";
        } break;
        case Node::block: {
            const Block *C=static_cast<const Block*>(N);
            unsigned qa = quoteArg(C->name);
            strm<<id<<C->name<<"(";
            for(size_t j=0, e=C->args.size(); j<e; j++){
                if(j>0)
                    strm<<",";
                if(j>=qa)
                    streamEscape(strm, C->args[j]);
                else
                    strm<<C->args[j];
            }
            if(C->children.size()) {
                strm<<") {\n";
                DBDASTPrinter(strm, C->children, indent+1);
                strm<<id<<"}\n";
            } else
                strm<<")\n";

        } break;
        }
    }
    return strm;
}
