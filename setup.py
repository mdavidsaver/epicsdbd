#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name="dbdlexparse",
      ext_modules=[
        Extension("dbdlexparse",
                  sources=['dbdlexparse.i','dbdlexer.cpp'],
                  swig_opts=['-c++'],
        )
      ],
)
