%module(directors="1") dbdlexparse

%include "std_except.i";
%include "std_string.i";
%include "std_vector.i";
%apply const std::string& {std::string* foo};


%{
#include "dbdlexer.h"
#include "dbdparser.h"
#include "pystream.h"
/* sub-class with non-pure virtual methods */

class PyDBDLexer : public DBDLexer {
public:
    PyDBDLexer() {}
    virtual ~PyDBDLexer() {}
    virtual void token(tokState_t st, DBDToken& tok) {}
};

class PyDBDParser : public DBDParser {
public:
    PyDBDParser() {};
    virtual ~PyDBDParser() {}
    virtual void parse_command(DBDToken& cmd, DBDToken& arg) {}
    virtual void parse_comment(DBDToken&) {}
    virtual void parse_code(DBDToken&) {}
    virtual void parse_block(DBDToken& name, blockarg_t&) {}
    virtual void parse_block_body_start() {}
    virtual void parse_block_body_end() {}
    virtual void parse_start(){};
    virtual void parse_eoi(){};
};

typedef DBDParser::blockarg_t blockarg_t;
%}

// translate python exception to C++ exception
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
// catch C++ exception and re-throw as python
%exception %{
  try {
    $action
  }catch(Swig::DirectorException& e){
    SWIG_fail;
  }catch(std::runtime_error& e){
    SWIG_exception_fail(SWIG_RuntimeError, e.what());
  }catch(std::exception& e){
    SWIG_exception_fail(SWIG_SystemError, e.what());
  }catch(...){
    SWIG_exception_fail(SWIG_SystemError, "Unknown C++ exception");
  }
%}

// build a List from vector<string> when passed to overloaded python
%typemap(directorin) blockarg_t& %{
{
  PyObject *x = PyList_New($1.size());
  $input = x;
  if(!$input) throw std::bad_alloc();
  for(size_t i=0, N=$1.size(); i<N; i++) {
    PyObject *obj = PyString_FromString($1[i].c_str());
    if(!obj) throw std::bad_alloc();
    PyList_SET_ITEM(x, i, obj);
  }
}
%}

class DBDToken
{
public:
    DBDToken();
    DBDToken(const DBDToken&);
    DBDToken(const std::string& v, unsigned l=1, unsigned c=0);
    std::string value;
    unsigned line, col;

    bool operator==(const DBDToken& o);
    bool operator!=(const DBDToken& o);

    %pythoncode %{
    def copy(self):
        return self.__class__(self)
    def __repr__(self):
        return 'DBDToken("%s", %d, %d)'%(self.value,self.line,self.col)
    __str__=__repr__
    %}
};

%feature("director") PyDBDLexer;
%feature("nodirector") PyDBDLexer::lex;
%rename(DBDLexer) PyDBDLexer;

class PyDBDLexer {
public:
    PyDBDLexer();
    virtual ~PyDBDLexer();

    enum tokState_t {
        tokInit, tokLit, tokQuote, tokEsc, tokBare, tokCode, tokComment, tokEOI
    };
    static const char* tokStateName(tokState_t S);

    virtual void reset();

    bool lexDebug;

    virtual void token(tokState_t, DBDToken&);
    %extend {
    void lex(PyObject *o)
    {
        pystreambuf buf(o);
        std::istream strm(&buf);
        $self->lex(strm);
    }
    }
};

%feature("director") PyDBDParser;
%feature("nodirector") PyDBDParser::token;
%feature("nodirector") PyDBDParser::lex;
%rename(DBDParser) PyDBDParser;

class PyDBDParser : public PyDBDLexer {
public:
    PyDBDParser();
    virtual ~PyDBDParser();

    virtual void reset();

    virtual void lex(std::istream&);

    inline unsigned depth() const;

    bool parDebug;

    virtual void parse_command(DBDToken& cmd, DBDToken& arg);
    virtual void parse_comment(DBDToken&);
    virtual void parse_code(DBDToken&);
    virtual void parse_block(DBDToken& name, blockarg_t&);
    virtual void parse_block_body_start();
    virtual void parse_block_body_end();
    virtual void parse_start();
    virtual void parse_eoi();

    %extend {
    void lex(PyObject *o)
    {
        pystreambuf buf(o);
        std::istream strm(&buf);
        $self->lex(strm);
    }
    }
};
