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


def bounded_model_check(k, atoms, state, tmp, prop):
    # create variables
    r = dict()

    # initialize all variables
    for atom in atoms:
        r[atom] = []
        for j in range(k + 1):
            r[atom].append(Int(f"{atom}_iter_{j}"))

    # parse property
    rprop = ""
    for c in prop:
        if c in atoms:
            rprop += "r[" + c + "][i]"
        else:
            rprop += c

    # initialize solver
    s = Solver()


    # initial state, states[0] by default the initial state
    init_state = []
    for atom in atoms:
        init_state.append(r[atom][0] == state[atom]["l0"]["value"])
    s.add(And(* init_state))

    # initialize tmp vars
    for t in tmp.keys():
        r[t] = [Int(f'{t}_iter_{j}' for j in range(k+1))]

    i = 0
    tmp_formulae = {}
    t = f't{i}'
    while t in tmp:
        instr = tmp[t]
        if instr['op'] == 'Load':
            for i in range(1,k):
                tmp_formulae[t][i] = Kind.EQUAL(r[t][i], r[instr['source']][i-1])
        else:
            for i in range(1,k):
                if instr['left'].isdecimal():
                    left = int(instr['left'])
                else:
                    left = tmp_formulae[instr['left']][i]
                if instr['right'].isdecimal():
                    right = int(instr['right'])
                else:
                    right = tmp_formulae[instr['right']][i]
                if instr['op'] == 'Add':
                    tmp_formulae[t][i] = Kind.EQUAL(r[t][i], Add(left, right))
                elif instr['op'] == 'Mult':
                    tmp_formulae[t][i] = Kind.EQUAL(r[t][i], Mult(left, right))
                elif instr['op'] == 'Sub':
                    tmp_formulae[t][i] = Kind.EQUAL(r[t][i], Sub(left, right))
        i += 1
        t = f't{i}'

    # transitions
    possible_labels = set()
    possible_labels.add('l0')
    for i in range(1,k):
        formula = {}
        for label in possible_labels:
            for atom in atoms:
                formula[atom] = []
                if label in state[atom]:
                    formula[atom].append(Kind.EQUAL(r[atom][i], tmp_formulae[state[atom][label]][i]))
                else:
                    formula[atom].append(Kind.EQUAL(r[atom][i], r[atom][i-1]))
        s.add(Or(* [f for f in formula.values()]))
        
        new_labels = []
        for label in possible_labels:
            for sl in succs[label]:
                new_labels.append(sl)
        possible_labels = set(new_labels)

    # property: AG (x < 20)
    if k == 0:
        i = 0
        s.add(eval(rprop))
    else:
        props = []
        for i in range(k+1):
            props.append(eval(rprop))
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
            print(f"STEP {j}:  ", end ="")
            for atom in atoms:
                print(atom + '=' + m[r[atom][j]] + '; ', end='')
            print("")

            # break if we find counterexample early
            if eval(prop.replace("x", "m[r['x'][j]]")):
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
