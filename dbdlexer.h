#ifndef DBDLEXER_HPP
#define DBDLEXER_HPP

#include <istream>
#include <ostream>
#include <string>
#include <algorithm>

struct DBDToken
{
    DBDToken() :line(0), col(0) {}
    DBDToken(const std::string& v, unsigned l=1, unsigned c=0)
        :value(v), line(l), col(c)
    {}
    std::string value;
    unsigned line, col;

    inline void push_back(std::string::value_type v)
    { value.push_back(v); }
    inline size_t size() const {return value.size();}
    void take(DBDToken& o)
    {
        value.clear();
        value.swap(o.value);
        line = o.line;
        col = o.col;
    }
    void reset()
    {
        value.clear();
        line = col = 0;
    }
};

std::ostream& operator<<(std::ostream&, const DBDToken&);

/** @brief DB/DBD lexer
 *
 @code
  tokBare   : [a-zA-Z0-9_\-+:.\[\]<>;]
  tokQuote  : '"' *([^"\n\\]|(\\.)) '"'
  tokCode   : '%' [^\n\r]*
  tokComment: '#' [^\n\r]*
  tokLit    : [{}(),]
 @endcode
 */
class DBDLexer
{
public:
    DBDLexer();

    /** @brief Tokenize an input stream
     *
     @throws std::runtime_error for invalid charator or unexpected EoI
     @throws std::logic_error for internal state errors
     */
    virtual void lex(std::istream&);

    virtual void reset();

    enum tokState_t {
        tokInit, tokLit, tokWS, tokQuote, tokEsc, tokBare, tokCode, tokComment, tokEOI
    } tokState;
    static const char* tokStateName(tokState_t S);

    bool lexDebug;

    DBDToken tok;
protected:
    /** @brief Lexer callback
     *
     * Called for each token.
     * inspect @var tokState and @var tok
     */
    virtual void token()=0;

private:
    void doToken(tokState_t next);
    void setLine();
    unsigned line, col;
};

#endif // DBDLEXER_HPP
