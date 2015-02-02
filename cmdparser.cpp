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
        for(unsigned i=0; i<depth; i++)
            strm<<"  ";
    }

    virtual void parse_command()
    {
        pad();
        strm<<CoBtoken.value<<" \"";
        streamEscape(strm, tok.value)<<"\"\n";
    }

    virtual void parse_comment()
    { pad(); strm<<"#"<<tok.value<<"\n";}

    virtual void parse_code()
    { pad(); strm<<"%"<<tok.value<<"\n";}

    virtual void parse_block()
    {
        pad();
        strm<<CoBtoken.value<<"(";
        for(size_t i=0, e=blockargs.size(); i<e; i++)
        {
            if(i>0)
                strm<<", ";
            streamEscape(strm, blockargs[i]);
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
