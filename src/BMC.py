# Model for a general Kripke Structure, for example
#   k = 3
#   atoms = ["a", "b"]
#   states = ["00", "01", "10", "11"]
#   trans = {"00":["01"], "01":["10"], "10":["11"]}
#   state[0] is by default the initial state

# Property: AG (~a1 or ~a2 ... ~an) we will never reach state 11...1
#   11...1 is always the bad state


import sys
import json
import cvc5
from cvc5 import Kind
from cvc5.pythonic import *


def bounded_model_check(k, atoms, state, tmp, property):
    # create variables
    r = dict()

    # initialize all variables
    for atom in atoms:
        r[atom] = []
        for j in range(k + 1):
            r[atom].append(Int(f"{atom}_iter_{j}"))

    # parse property
    tmp_property = ""
    for c in property:
        if c in atoms:
            property += "r[" + c + "][i]"
        else:
            property += c
    property = tmp_property

    # initialize solver
    s = Solver()


    # initial state, states[0] by default the initial state
    init_state = []
    for atom in atoms:
        init_state.append(r[atom][0] == state[atom]["l0"]["value"])
    s.add(And(* init_state))


    # transitions
    for j in range(k):
        formula = []

        for source in trans:
            dests = trans[source]
            for dest in dests:
                formula_sub = []
                for i in range(len(dest)):
                    if source[i] == "1":
                        formula_sub.append(r[i][j])
                    else:
                        formula_sub.append(Not(r[i][j]))
                    if dest[i] == "1":
                        formula_sub.append(r[i][j+1])
                    else:
                        formula_sub.append(Not(r[i][j+1]))
                formula.append(And(* formula_sub))

        s.add(Or(* formula))


    # property: AG (x < 20)
    if k == 0:
        i = 0
        s.add(eval(property))
    else:
        props = []
        for i in range(k+1):
            props.append(eval(property))
        s.add(Or(*props))


    # solve the SMT problem
    try:
        assert sat == s.check()
    except AssertionError:
        print(f"Property satisfied within {k} steps")
        return False
    else:
        # retrieve the satisfying assignment
        m = s.model()

        # print out each step
        for j in range(k + 1):
            s = 1
            print(f"STEP {j}: ", end ="")
            for i in range(len(atoms)):
                if m[r[i][j]]:
                    print("1", end="")
                    s = s * 1
                else:
                    print("0", end="")
                    s = s * 0
            print("")

            # break if we find counterexample early
            if s == 1:
                print(f"Counterexample found at STEP {j}")
                return True


def iterative_bounded_model_check(limit, atoms, states, trans):
    for k in range(limit+1):
        if not bounded_model_check(k, atoms, states, trans):
            continue
        else:
            return



if __name__ == "__main__":
    # uncomment the lines below to test some examples!!!

    # limit = 5
    # atoms = ["a", "b"]
    # states = ["00", "01", "10", "11"]
    # trans = {"00":["01"], "01":["10"], "10":["11"]}
    # iterative_bounded_model_check(limit, atoms, states, trans)

    # limit = 5
    # atoms = ["a", "b", "c", "d"]
    # states = ["0000", "0001", "0010", "0101", "0011", "1111"]
    # trans = {"0000": ["0001"], "0001": ["0101","0010"], "0101": ["0011"], "0010":["0011","0000"], "0011":["1111"]}
    # iterative_bounded_model_check(limit, atoms, states, trans)

    model_path = sys.argv[1]
    limit = int(sys.argv[2])

    f = open(model_path)
    model = json.load(f)
    atoms = model["atoms"]
    states = model["state"]
    trans = model["pred_bb"]
    succs = model["succ_bb"]
    tmp = model["tmp"]

    property = "And(x < 20, y == 2)"    # we love not sanitizing our input

    iterative_bounded_model_check(limit, atoms, states, tmp, property)

    f.close()
