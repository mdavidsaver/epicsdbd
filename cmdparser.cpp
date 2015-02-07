#include <fstream>
#include <iostream>

#include "dbdparser.h"
#include "util.h"

namespace {
struct PrintingParser : public DBDParser
{
    std::ostream& strm;
    PrintingParser(std::ostream& s) : strm(s) {}

    void pad()
    {
        for(unsigned i=0; i<depth(); i++)
            strm<<"  ";
    }

    virtual void parse_command(DBDToken& cmd, DBDToken& arg)
    {
        pad();
        strm<<cmd.value<<" \"";
        streamEscape(strm, arg.value)<<"\"\n";
    }

    virtual void parse_comment(DBDToken& tok)
    { pad(); strm<<"#"<<tok.value<<"\n";}

    virtual void parse_code(DBDToken& tok)
    { pad(); strm<<"%"<<tok.value<<"\n";}

    virtual void parse_block(DBDToken& name, blockarg_t& args)
    {
        pad();
        strm<<name.value<<"(";
        for(size_t i=0, e=args.size(); i<e; i++)
        {
            if(i>0)
                strm<<", ";
            streamEscape(strm, args[i]);
        }
        strm<<")\n";
    }

    virtual void parse_block_body_start()
    { pad(); strm<<tok.value<<"\n"; }

    virtual void parse_block_body_end()
    { pad(); strm<<tok.value<<"\n"; }
};

}//namespace

int main(int argc, char *argv[]) {
    std::istream *istrm = &std::cin;
    std::ostream *ostrm = &std::cout;
    std::ifstream infile;
    std::ofstream outfile;
    if(argc>1) {
        infile.open(argv[1]);
        istrm = &infile;
    }
    if(argc>2) {
        outfile.open(argv[2]);
        ostrm = &outfile;
    }

    PrintingParser P(*ostrm);
    //P.parDebug = true;
    try{
        P.lex(*istrm);
        std::cerr<<"Success\n";
        return 0;
    }catch(std::exception& e) {
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}
