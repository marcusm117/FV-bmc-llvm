# FV_modelchecker
## Dependencies
[cvc5 documentation](https://cvc5.github.io/)
* Install: `pip install cvc5`
* Import: `import cvc5`   

[Z3 documentation](https://github.com/Z3Prover/z3/wiki)
* Install: `pip install z3-solver`
* Import: `from z3 import *`


## How To Use
### BMC for a general Kripke Structure
1. Create an input model.json, for exmaple
```
{
"atoms" : ["a", "b", "c", "d"],
"states" : ["0000", "0001", "0010", "0101", "0011", "1111"],
"trans" : {"0000": ["0001"],
	"0001": ["0101","0010"], 
	"0101": ["0011"], 
	"0010":["0011","0000"], 
	"0011":["1111"]}
}
```
2. The State Representation `"0010"` means that `a=0, b=0, c=1, d=0` for this State
3. The Initial State is by default `states[0]` and the Error State is by default `"1111"`
4. The Transition Representation `"0001: ["0101","0010"]"` means that from State `"0001"`, we can go to Sate `"0101"` and `"0010"` 
5. `cd src`
6. `python3 BMC_general.py [model.json] [limit]`, for example
7. When we run `python3 BMC_general.py test_model.json 5`, the result should be
```
Property satisfied within 0 steps
Property satisfied within 1 steps
Property satisfied within 2 steps
Property satisfied within 3 steps
STEP 0: 0000
STEP 1: 0001
STEP 2: 0101
STEP 3: 0011
STEP 4: 1111
Counterexample found at STEP 4
```
   
     
### Translator from LLVM IR to Intermediate Model



## Limitations
* Can only `store` each variable once per basic block
* Only supports basic LLVM instructions: no pointers, structs, or calls
* Only one LLVM function at a time
* Only AG properties