#ifndef DBDLEXER_HPP
#define DBDLEXER_HPP

#include <istream>
#include <ostream>
#include <string>
#include <algorithm>

struct DBDToken
{
    DBDToken() :line(1), col(0) {}
    DBDToken(const std::string& v, unsigned l=1, unsigned c=0)
        :value(v), line(l), col(c)
    {}
    std::string value;
    unsigned line, col;

    inline void newline() {line++; col=1;}
    inline void inc() {col++;}
    inline void push_back(std::string::value_type v)
    { value.push_back(v); }
    void swap(DBDToken& o)
    {
        value.swap(o.value);
        std::swap(line, o.line);
        std::swap(col, o.col);
    }
    void reset()
    {
        value.clear();
    }
};

std::ostream& operator<<(std::ostream&, const DBDToken&);

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

    DBDToken tok;
protected:
    virtual void token()=0;
};

#endif // DBDLEXER_HPP
