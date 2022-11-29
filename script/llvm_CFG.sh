#!/bin/sh
clang -S -emit-llvm $1.c -o $1.ll
opt -dot-cfg -disable-output -enable-new-pm=0 $1.ll
dot -Tpng -omain.png .main.dot
