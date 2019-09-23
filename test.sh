#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 "0;"
try 42 "42;"
try 21 '5+20-4;'
try 41 " 12 + 34 - 5 ;"
try 47 "5+6 * 7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 16 "-4 + 20;"
try 10 "+24 + (-14);"
try 3 "+3;"
try 10 "-3 * +5 + 25;"
try 1 "1 < 4;"
try 0 " -4 > 4;"
try 1 " -1 <= -1;"
try 1 " 1 == +1;"
try 0 "-1 == 4;"
try 1 "-10 != 3;"
try 0 "11 !=  11;"
try 1 "a =  +1; a;"
try 4 "z = 2 * 2; z;"
try 1 "h = 4 + (1-3); h > 0;"
try 3 "a = 3; return a; return 4;"
try 10 "b = 2 * 5; return b;"
try 5 "hogehoge = 5; return hogehoge;"
try 5 "i = 0; while (i < 5) i = i + 1; return i;"
try 6 "hoge = 0; while (hoge < 6) hoge = hoge + 1; return hoge;"
try 5 "i = 10; if (i == 10) return 5; else return 100;"
try 10 "i = 10;
if (i <10) return 4;
else return 10;"
try 10 "for (hoge = 0; hoge < 10; hoge = hoge + 1) hoge = hoge + 1; return hoge; "

echo OK
