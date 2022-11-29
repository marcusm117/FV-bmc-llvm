# Various resources on how to generate LLVM CFG

## Source #1: incorrect
[source](https://yunmingzhang.wordpress.com/2014/11/12/notes-on-llvm-getting-started-with-program-analysis/)
1. `clang -emit-llvm hello.c -c -o hello.bc`
1. `opt -dot-cfg-only hello.bc` (a cfg.main.dot file was generated)
1. use Graphviz ( http://www.graphviz.org) to show the graph of the output file

## Source #2: also incorrect
1. create .bc file: `clang -emit-llvm -c testcfg.c`
1. create .dot file: `opt -dot-cfg testcfg.bc`
1. create png,pdf: `dot -Tpng -o main.png cfg.main.dot`

## Source #3: correct!
[source](https://wisesciencewise.wordpress.com/2022/10/03/steps-to-generate-llvm-call-flow-graphcfg/)
1. `clang -S -emit-llvm file.c -o file.ll`
1. `opt -dot-cfg -disable-output -enable-new-pm=0 file.ll`
1. `dot -Tpng -ofunction.png .function.dot`
