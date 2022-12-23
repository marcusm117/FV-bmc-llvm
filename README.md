# FV_modelchecker
## To Do
* Ask Franjo for help on figuring out transition relation for arbitrary program
  * In general, the problem of generating a model from the source code
* @gschare: get cvc5 working on test.py example
* Study Python API
* Get two-bit counter example working
* Study other projects that use cvc5

## Resources
[cvc5 documentation](https://cvc5.github.io/)
* [Python API](https://cvc5.github.io/docs/cvc5-1.0.2/api/python/python.html)
  * Install: `pip install cvc5` and import as `import cvc5`
* [C++ API](https://cvc5.github.io/docs/cvc5-1.0.2/api/cpp/cpp.html)

## Interface
* Toy Model Checker
  * Graphic interface where users can fill in variables
    * AG(x == y), fill in x and y with LLVM IR variables
