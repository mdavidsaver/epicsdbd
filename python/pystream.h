#ifndef PYSTREAM_H
#define PYSTREAM_H

#include <Python.h>

#include <istream>
#include <vector>

class pystreambuf : public std::streambuf
{
public:
  PyObject *obj;
  pystreambuf(PyObject *o);
  ~pystreambuf();

  std::vector<char> inbuf;

  virtual int_type underflow();
};

#endif // PYSTREAM_H
