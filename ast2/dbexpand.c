#include <stdio.h>

#include "dbdast.h"

int main(int argc, char *argv[])
{
    const char *iname="<stdin>";
    FILE *fin = stdin, *fout = stdout;
    if(argc>1) {
        iname = argv[1];
        fin = fopen(argv[1], "r");
        if(!fin) {
            fprintf(stderr, "Failed to open %s\n",argv[1]);
            return 1;
        }
    }
    if(argc>2) {
        fout = fopen(argv[2], "w");
        if(!fout) {
            fprintf(stderr, "Failed to open %s\n",argv[2]);
            return 1;
        }
    }

    DBDFile *file = DBDParseFileP(fin, iname);
    if(!file) {
        fprintf(stderr,"Failed to read %s\n",argv[1]);
    } else {
        DBDShow(&file->common, fout, 0);
    }
    DBDFreeNode(&file->common);
    return 0;
}
