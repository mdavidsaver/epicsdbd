#include <cstdio>

#include <iostream>
#include <istream>
#include <fstream>

#include "dbdast.h"

int main(int argc, char *argv[])
{
    FILE *fout = stdout;
    if(argc<2) {
        std::cerr<<"Usage: "<<argv[0]<<" <infile> [outfile]\n";
        return 1;
    }
    if(argc>2) {
        fout = fopen(argv[2], "w");
        if(!fout) {
            std::cerr<<"Failed to open "<<argv[2]<<"\n";
            return 1;
        }
    }

    DBDFile *file = DBDParseFile(argv[1]);
    if(!file) {
        std::cerr<<"Failed to read "<<argv[1]<<"\n";
    } else {
        DBDShow(&file->common, fout, 0);
    }
    DBDFreeNode(&file->common);
    return 0;
}
