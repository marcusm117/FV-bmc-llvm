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
  * Mac Installation
    1. `python3 -m pip install cvc5` and import as `import cvc5`
  * if that doesn't work, install manually:
    1. git clone https://github.com/cvc5/cvc5
    2. cd cvc5
    3. make sure the pip packages: toml, scikit-build, and Cython are installed
    4. ./configure.sh --python-bindings --auto-download
    5. cd build
    6. make -j cvc5_python_api
    7. export PYTHONPATH="<path-to-local-cvc5-repo>/build/src/api/python/:$PYTHONPATH"
* [C++ API](https://cvc5.github.io/docs/cvc5-1.0.2/api/cpp/cpp.html)

## Interface
* Toy Model Checker
  * Graphic interface where users can fill in variables
    * AG(x == y), fill in x and y with LLVM IR variables
