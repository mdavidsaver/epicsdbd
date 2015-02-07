#include <cstdio>

#include <iostream>

#include "dbdlexer.h"
#include "cfstream.h"

namespace {
struct PrintingLexer : public DBDLexer
{
    std::ostream& strm;
    PrintingLexer(std::ostream& s) : strm(s) {}
    virtual void token(tokState_t state, DBDToken& token)
    {
        strm<<DBDLexer::tokStateName(state)<<"\t"<<token<<"\n";
    }
};
}

int main(int argc, char *argv[]) {
    cfile_streambuf ifs, ofs;
    if(argc>1 && argv[1][0]!='-') {
        FILE *fp=fopen(argv[1], "r");
        if(!fp) {
            perror("Failed to open input file");
            return 1;
        }
        ifs.set(fp);
    } else {
        ifs.set(stdin);
    }
    if(argc>2 && argv[2][0]!='-') {
        FILE *fp=fopen(argv[2], "w");
        if(!fp) {
            perror("Failed to open output file");
            return 1;
        }
        ofs.set(fp);
    } else {
        ofs.set(stdout);
    }

    std::istream istrm(&ifs);
    std::ostream ostrm(&ofs);

    PrintingLexer P(ostrm);
    //P.lexDebug = true;
    try{
        P.lex(istrm);
        std::cerr<<"Success\n";
        return 0;
    }catch(std::exception& e) {
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}
