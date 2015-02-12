#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest

import epicsdbd as DLP
from epicsdbd import DBDToken, DBDLexer
from cStringIO import StringIO

class StoreLex(DBDLexer):
    def __init__(self):
        super(StoreLex,self).__init__()
        self.result = []
    def token(self, S, T):
        self.result.append((S,T.copy()))

class TestLex(unittest.TestCase):
    def test_line(self):
        S = StringIO("hello world")
        L = StoreLex()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(L.result, [
            (DBDLexer.tokBare, DBDToken("hello",1,1)),
            (DBDLexer.tokBare, DBDToken("world",1,7)),
            (DBDLexer.tokEOI, DBDToken("",1,11)),
        ])

    def test_menu(self):
        _T = """menu(waveformPOST) {
        choice(waveformPOST_Always,"Always")
        choice(waveformPOST_OnChange,"On Change")
}"""

        S = StringIO(_T)
        L = StoreLex()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(L.result, [
            (DBDLexer.tokBare, DBDToken("menu", 1, 1)),
            (DBDLexer.tokLit, DBDToken("(", 1, 5)),
            (DBDLexer.tokBare, DBDToken("waveformPOST", 1, 6)),
            (DBDLexer.tokLit, DBDToken(")", 1, 18)),
            (DBDLexer.tokLit, DBDToken("{", 1, 20)),
            (DBDLexer.tokBare, DBDToken("choice", 2, 9)),
            (DBDLexer.tokLit, DBDToken("(", 2, 15)),
            (DBDLexer.tokBare, DBDToken("waveformPOST_Always", 2, 16)),
            (DBDLexer.tokLit, DBDToken(",", 2, 35)),
            (DBDLexer.tokQuote, DBDToken("Always", 2, 36)),
            (DBDLexer.tokLit, DBDToken(")", 2, 44)),
            (DBDLexer.tokBare, DBDToken("choice", 3, 9)),
            (DBDLexer.tokLit, DBDToken("(", 3, 15)),
            (DBDLexer.tokBare, DBDToken("waveformPOST_OnChange", 3, 16)),
            (DBDLexer.tokLit, DBDToken(",", 3, 37)),
            (DBDLexer.tokQuote, DBDToken("On Change", 3, 38)),
            (DBDLexer.tokLit, DBDToken(")", 3, 49)),
            (DBDLexer.tokLit, DBDToken("}", 4, 1)),
            (DBDLexer.tokEOI, DBDToken("", 4, 1))
        ])

    def test_cmd(self):
        _T="""breaktable(typeJdegF) {
        0.000000 32.000000
        425.209311 161.000000
        1218.865107 390.000000
        2893.652879 869.000000
        3603.245090 1068.000000
        4098.753854 1201.000000
} #comment"""


        S = StringIO(_T)
        L = StoreLex()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(L.result, [
            (DBDLexer.tokBare, DBDToken("breaktable", 1, 1)),
            (DBDLexer.tokLit, DBDToken("(", 1, 11)),
            (DBDLexer.tokBare, DBDToken("typeJdegF", 1, 12)),
            (DBDLexer.tokLit, DBDToken(")", 1, 21)),
            (DBDLexer.tokLit, DBDToken("{", 1, 23)),
            (DBDLexer.tokBare, DBDToken("0.000000", 2, 9)),
            (DBDLexer.tokBare, DBDToken("32.000000", 2, 18)),
            (DBDLexer.tokBare, DBDToken("425.209311", 3, 9)),
            (DBDLexer.tokBare, DBDToken("161.000000", 3, 20)),
            (DBDLexer.tokBare, DBDToken("1218.865107", 4, 9)),
            (DBDLexer.tokBare, DBDToken("390.000000", 4, 21)),
            (DBDLexer.tokBare, DBDToken("2893.652879", 5, 9)),
            (DBDLexer.tokBare, DBDToken("869.000000", 5, 21)),
            (DBDLexer.tokBare, DBDToken("3603.245090", 6, 9)),
            (DBDLexer.tokBare, DBDToken("1068.000000", 6, 21)),
            (DBDLexer.tokBare, DBDToken("4098.753854", 7, 9)),
            (DBDLexer.tokBare, DBDToken("1201.000000", 7, 21)),
            (DBDLexer.tokLit, DBDToken("}", 8, 1)),
            (DBDLexer.tokComment, DBDToken("comment", 8, 3)),
            (DBDLexer.tokEOI, DBDToken("", 8, 10)),
        ])

    def test_cont(self):
        """Mid-word quotes treated as two tokens
        """
        S = StringIO('test"ing"')
        L = StoreLex()
        L.lex(S)

        self.assertEqual(L.result, [
            (DBDLexer.tokBare, DBDToken("test", 1, 1)),
            (DBDLexer.tokQuote, DBDToken("ing", 1, 5)),
            (DBDLexer.tokEOI, DBDToken("", 1, 9)),
        ])

    def test_oops(self):
        """Invalid charactor
        """
        S = StringIO('test \x7f')
        L = StoreLex()
        self.assertRaises(RuntimeError, L.lex, S)

    def test_dangling(self):
        """Missing quote
        """
        S = StringIO('hello "world')
        L = StoreLex()
        self.assertRaises(RuntimeError, L.lex, S)

    def test_utf(self):
        """Accept 8-bit strings in quotes
        """
        D=u'"徐广磊" says hi'.encode('utf8')
        #D='"michael" says hi'
        S = StringIO(D)
        L = StoreLex()
        #L.lexDebug = True
        L.lex(S)

        self.assertEqual(len(u'徐广磊'.encode('utf8')), 9)

        self.assertEqual(L.result, [
            (DBDLexer.tokQuote, DBDToken(u'徐广磊'.encode('utf8'), 1, 1)),
            (DBDLexer.tokBare, DBDToken("says", 1, 13)),
            (DBDLexer.tokBare, DBDToken("hi", 1, 18)),
            (DBDLexer.tokEOI, DBDToken("", 1, 19)),
        ])

if __name__=="__main__":
    unittest.main()
