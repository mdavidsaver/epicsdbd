#include <cstring>
#include <cassert>

#include <fstream>
#include <sstream>

#include <errlog.h>

#include "dbdast.h"

#include "dbdparser.h"

namespace {
class DBDASTParser : public DBDParser
{
public:
    DBDASTParser()
        :totalalloc(0)
    {
        fakeroot = (DBDBlock*)calloc(1, sizeof(*fakeroot));
        if(!fakeroot)
            throw std::bad_alloc();
    }

    virtual ~DBDASTParser()
    {
        DBDFreeNode(&fakeroot->common);
    }

    //virtual void reset();

    virtual void parse_command()
    {
        assert(stack.size()==depth+1);

        size_t clen = CoBtoken.value.size(),
               alen =tok.value.size();
        if(clen>0xffffff||alen>0xffffff)
            throw std::bad_alloc();
        DBDStatement *stmt=(DBDStatement*)calloc(1, sizeof(*stmt)+alen+clen);
        totalalloc += sizeof(*stmt)+alen+clen;
        if(!stmt)
            throw std::bad_alloc();
        stmt->common.type = DBDNodeStatement;

        stmt->cmd = stmt->_alloc;
        stmt->arg = stmt->cmd+clen+1;
        stmt->common.line = CoBtoken.line;
        stmt->common.col = CoBtoken.col;
        memcpy(stmt->cmd, CoBtoken.value.c_str(), clen);
        memcpy(stmt->arg, tok.value.c_str(), alen);

        ellAdd(&stack.back()->children, &stmt->common.node);
    }

    void nest(char type)
    {
        assert(stack.size()==depth+1);
        size_t len = tok.value.size();
        if(len>0xffffff)
            throw std::bad_alloc();
        DBDNest *stmt=(DBDNest*)calloc(1, sizeof(*stmt)+len+1);
        totalalloc += sizeof(*stmt)+len+1;
        if(!stmt)
            throw std::bad_alloc();

        stmt->common.type = DBDNodeNest;
        stmt->common.line = CoBtoken.line;
        stmt->common.col = CoBtoken.col;
        stmt->line[0] = type;
        memcpy(stmt->line+1, tok.value.c_str(), len);

        ellAdd(&stack.back()->children, &stmt->common.node);
    }

    virtual void parse_comment(){nest('#');}
    virtual void parse_code(){nest('%');}

    virtual void parse_block()
    {
        assert(stack.size()==depth+1);
        size_t nlen = CoBtoken.value.size(), alen=nlen,
               bcnt = blockargs.size();
        if(nlen>0xff || bcnt>DBDBlockMaxArgs)
            throw std::bad_alloc();
        for(unsigned i=0; i<bcnt; i++) {
            size_t esize = blockargs[i].size()+1;
            if(size_t(-1)-alen<esize)
                throw std::bad_alloc();
            alen += esize;
        }
        DBDBlock *stmt=(DBDBlock*)calloc(1, sizeof(*stmt)+alen);
        totalalloc += sizeof(*stmt)+alen;
        if(!stmt)
            throw std::bad_alloc();
#ifndef NDEBUG
        const char * const buflim = (char*)stmt + sizeof(*stmt)+alen;
#endif

        stmt->common.type = DBDNodeBlock;
        stmt->common.line = CoBtoken.line;
        stmt->common.col = CoBtoken.col;
        stmt->name = stmt->_alloc;
        stmt->nargs = bcnt;
        memcpy(stmt->name, CoBtoken.value.c_str(), nlen);

        char *buf = stmt->_alloc+nlen+1;
        for(size_t i = 0;  i<bcnt; i++) {
            assert(buf<buflim);
            size_t esize = blockargs[i].size();
            stmt->args[i] = buf;
            memcpy(buf, blockargs[i].c_str(), esize);
            buf += esize+1;
        }

        ellAdd(&stack.back()->children, &stmt->common.node);
    }

    virtual void parse_block_body_start()
    {
        assert(stack.size()==depth+1);
        DBDBlock *stmt=stack.back();
        assert(ellCount(&stmt->children)>0);
        DBDNode *back=(DBDNode*)ellLast(&stmt->children);
        assert(back->type==DBDNodeBlock);

        stack.push_back((DBDBlock*)back);
    }

    virtual void parse_block_body_end()
    {
        stack.pop_back();
    }

    virtual void parse_start()
    {
        stack.push_back(fakeroot);
    }

    virtual void parse_eoi()
    {
        assert(stack.size()==1);
        assert(stack.at(0)==fakeroot);
        stack.pop_back();
    }

    std::vector<DBDBlock*> stack;

    DBDBlock *fakeroot;
    size_t totalalloc;
};

}//namespace

DBDFile *DBDParseFile(const char* fname)
{
    DBDFile *file = (DBDFile*)calloc(1, sizeof(*file)+strlen(fname));
    if(!file)
        return NULL;
    strcpy(file->name, fname);

    try{
        std::ifstream fs(fname);
        DBDASTParser P;
        P.lex(fs);
        errlogPrintf("Total alloc %lu", (unsigned long)P.totalalloc);
        ellConcat(&file->entries, &P.fakeroot->children);
    } catch(std::exception& e) {
        free(file);
        file = NULL;
        errlogPrintf("Parsing error: %s", e.what());
    }

    return file;
}

DBDFile *DBDParseMemory(const char *buf, const char *fname)
{
    return NULL;
}
