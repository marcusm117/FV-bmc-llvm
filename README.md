# FV_modelchecker
## How To Use
To turn LLVM IR file into bitcode:
`llvm-as test.ll –o test.bc`

`make`
`./mc test.bc`

## To Do
* Study other projects that use cvc5
* Greg: make a demo program translating LLVM instructions into SMT problem
* Marcus: make a BMC for general transition systems
* + start working on the slides

## Resources
[cvc5 documentation](https://cvc5.github.io/)
* [Python API](https://cvc5.github.io/docs/cvc5-1.0.2/api/python/python.html)
  * Install: `pip install cvc5` and import as `import cvc5`
* [C++ API](https://cvc5.github.io/docs/cvc5-1.0.2/api/cpp/cpp.html)

## Interface
* Toy Model Checker
  * Graphic interface where users can fill in variables
    * AG(x == y), fill in x and y with LLVM IR variables

## Limitations
* Can only `store` each variable once per basic block
* Only supports basic LLVM instructions: no pointers, structs, or calls
* Only one LLVM function at a time
* Only AG properties
