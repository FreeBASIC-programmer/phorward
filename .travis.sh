#!/bin/sh
set -ex
./configure
make
sudo make install
pparse -G "f: /[0-9]+/@int | '(' e ')' ; mul@: t '*' f ; t: mul@ | f; add@: e '+' t; e: add@ | t ;"  "1+2*3+4*5"
pparse -G "a: Integer; b: 'Hallo Welt'; c: \"Tri tra\"; d: /[0-9]+/@int ;"  "1234"
