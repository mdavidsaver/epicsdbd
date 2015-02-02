#ifndef DBDLEXER_HPP
#define DBDLEXER_HPP

#include <istream>
#include <string>

struct DBDToken
{
    char *first, *last;
    unsigned line, col;
};

class DBDLexer
{
public:
    DBDLexer();

    void lex(std::istream&);

    virtual void reset();

    enum tokState_t {
        tokInit, tokLit, tokWS, tokQuote, tokEsc, tokBare, tokCode, tokComment, tokEOI
    } tokState;
    static const char* tokStateName(tokState_t S);

    std::string tok;
    unsigned line, col;
protected:
    virtual void token()=0;
};

#endif // DBDLEXER_HPP
