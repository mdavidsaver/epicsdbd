#!/usr/bin/env python

import unittest

import dbdlexparse as DLP
from dbdlexparse import DBDToken, DBDLexer
from cStringIO import StringIO

class StoreParser(DLP.DBDParser):
    def __init__(self):
        super(StoreParser,self).__init__()
        self.results, self._stack = [], []
        self._stack.append(self.results)
        self.started = self.ended = False
    def parse_command(self, C, A):
        self.results.append(('cmd',C.copy(),A.copy()))
    def parse_comment(self, C):
        self.results.append(('comm',C.copy()))
    def parse_code(self, C):
        self.results.append(('comm',C.copy()))
    def parse_block(self, N, A):
        self.results.append(('blk',N.copy(),A,[]))
    def parse_block_body_start(self):
        B = self.results[-1]
        self._stack.append(B[3])
        self.results = B[3]
    def parse_block_body_end(self):
        self._stack.pop()
        self.results = self._stack[-1]
    def parse_start(self):
        self.started = True
    def parse_eoi(self):
        self.ended = True

class TestParse(unittest.TestCase):
    def test_one(self):
        S = StringIO("hello world")
        L = StoreParser()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(len(L._stack), 1)
        self.assertEqual(L._stack[0], [
            ('cmd', DBDToken("hello", 1, 1), DBDToken("world", 1, 7)),
        ])

    def test_menu(self):
        _T = """menu(waveformPOST) {
        choice(waveformPOST_Always,"Always")
        choice(waveformPOST_OnChange,"On Change")
}"""

        S = StringIO(_T)
        L = StoreParser()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(len(L._stack), 1)
        self.assertEqual(L._stack[0], [
            ('blk', DBDToken("menu", 1, 1), ['waveformPOST'], [
                ('blk', DBDToken("choice", 2, 9), ['waveformPOST_Always', 'Always'], []),
                ('blk', DBDToken("choice", 3, 9), ['waveformPOST_OnChange', 'On Change'], [])]),
            ])

if __name__=="__main__":
    unittest.main()
