# FV_modelchecker
## Dependencies
[cvc5 documentation](https://cvc5.github.io/)
* Install: `pip install cvc5`
* Import: `import cvc5`   

[Z3 documentation](https://github.com/Z3Prover/z3/wiki)
* Install: `pip install z3-solver`
* Import: `from z3 import *`


## How To Use
To turn LLVM IR file into bitcode:
`llvm-as test.ll –o test.bc`

`make`
`./mc test.bc`


## Interface
* Toy Model Checker
  * Graphic interface where users can fill in variables
    * AG(x == y), fill in x and y with LLVM IR variables

## Limitations
* Can only `store` each variable once per basic block
* Only supports basic LLVM instructions: no pointers, structs, or calls
* Only one LLVM function at a time
* Only AG properties