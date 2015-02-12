EPICS DB/DBD file parser
========================

Building
--------

* Requires EPICS Base >=3.14.11
* CMake >=2.8

    $ git clone https://github.com/mdavidsaver/epicsdbd.git
    $ git submodule init
    $ mkdir build
    $ (cd build && cmake ..)
    $ make -C build

Testing

    $ ./build/cli/cmdlexer /usr/lib/epics/dbd/softIoc.dbd
    $ ./build/cli/cmdparser /usr/lib/epics/dbd/softIoc.dbd
    $ ./build/cli/cmdast1 /usr/lib/epics/dbd/softIoc.dbd
    $ ./build/cli/cmdast2 /usr/lib/epics/dbd/softIoc.dbd

Building Python module
----------------------

* Python >=2.7
* SWIG >=2.0

    $ cd python
    $ ./setup.py build_ext -i

Testing

    $ python test_lexer.py
    $ python test_parser.py
