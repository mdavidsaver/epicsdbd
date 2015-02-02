#include <fstream>
#include <iostream>

#include "dbdlexer.h"

namespace {
struct PrintingLexer : public DBDLexer
{
    std::ostream& strm;
    PrintingLexer(std::ostream& s) : strm(s) {}
    void token()
    {
        strm<<DBDLexer::tokStateName(tokState)<<"\t"<<tok<<"\n";
    }
};
}

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

    PrintingLexer P(*ostrm);
    try{
        P.lex(*istrm);
        std::cerr<<"Success\n";
        return 0;
    }catch(std::exception& e) {
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}
