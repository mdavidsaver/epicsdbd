#!/usr/bin/env python

from distutils.core import setup, Extension

from os import getcwd
from os.path import join

setup(name="epicsdbd",
      ext_modules=[
        Extension("_epicsdbd",
                  sources=['epicsdbd.i',
                           '../lib/dbdlexer.cpp',
                           '../lib/dbdparser.cpp',
                           'pystream.cpp'],
                  include_dirs=['../lib'],
                  swig_opts=['-c++','-I../lib'],
        ),
      ],
      py_modules = ['epicsdbd'],
)
