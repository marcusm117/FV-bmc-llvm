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
  * Mac Installation Guide for cvc5 Python Bindings
    1. `python3 -m pip install cvc5` and import as `import cvc5`
  * If that doesn't work, install manually:
    1. `git clone https://github.com/cvc5/cvc5`
    1. `cd cvc5`
    1. make sure the pip packages: toml, scikit-build, and Cython are installed
    1. `./configure.sh --python-bindings --auto-download`
    1. `cd build`
    1. make -j cvc5_python_api
    1. `make check`
    1. `make install`
    1. `export PYTHONPATH="<path-to-local-cvc5-repo>/build/src/api/python/:$PYTHONPATH"`
  * Note: these didn't work on ARM64 (M1) architecture (even for other language
    bindings). Google Colab was suitable as a workaround, though.
* [C++ API](https://cvc5.github.io/docs/cvc5-1.0.2/api/cpp/cpp.html)

## Interface
* Toy Model Checker
  * Graphic interface where users can fill in variables
    * AG(x == y), fill in x and y with LLVM IR variables
