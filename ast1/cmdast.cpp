#include <fstream>
#include <iostream>

#include "dbdast.h"

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

    DBDASTParser P;
    //P.parDebug = true;
    try{
        P.lex(*istrm);
        std::cerr<<"Success\n";
        DBDASTPrinter(*ostrm, P.tree);
        return 0;
    }catch(std::exception& e) {
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
}

