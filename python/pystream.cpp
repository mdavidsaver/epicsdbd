#include "pystream.h"

pystreambuf::pystreambuf(PyObject *o)
  :obj(o)
  ,inbuf(8)
{
    Py_INCREF(obj);
    setg(&inbuf[0], &inbuf[8], &inbuf[8]);    
}

pystreambuf::~pystreambuf()
{
    Py_DECREF(obj);
}

pystreambuf::int_type
pystreambuf::underflow()
{
    char_type *cur=gptr(), *back=egptr();
    if(cur<back) {
        return traits_type::to_int_type(*cur);
    }
    // buffer empty

    PyObject* buf = PyObject_CallMethod(obj, "read", "i", 1024);
    if(!buf) {
        PyErr_Print();
        PyErr_Clear();
        return traits_type::eof();
    }
    
    Py_buffer view;
    if(PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE)) {
        Py_DECREF(buf);
        PyErr_Print();
        PyErr_Clear();
        return traits_type::eof();
    }

    if(view.len==0) {
        PyBuffer_Release(&view);
        Py_DECREF(buf);
        return traits_type::eof();
    }
    
    try{
        inbuf.resize(view.len+8);
        memcpy(&inbuf[8], view.buf, view.len);
    }catch(...){
        PyBuffer_Release(&view);
        Py_DECREF(buf);
        throw;
    }
    PyBuffer_Release(&view);
    Py_DECREF(buf);
    
    setg(&inbuf[0], &inbuf[8], &inbuf[8+view.len]);
    return traits_type::to_int_type(*cur);
}
