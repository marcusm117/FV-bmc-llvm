# Model for a general Kripke Structure, for example
#   Atoms: a, b, c, d
#   States: 0000 (a=0, b=0, c=0, d=0), 0001, 0010, 1101, 1111
#   Initial State: 0000
#   Transitions: 0000->0001, 0001->0010, 0010->0001, 0001->1101, 1101->0000

# Property: AG (~a1 or ~a2 ... ~an) we will never reach state 11...1


import cvc5
from cvc5 import Kind
from cvc5.pythonic import *


def bounded_model_check(k, atoms, states, trans):
    # create variables
    r = []

    # initialize all variables
    for i in range(len(atoms)):
        r.append([])
        for j in range(k + 1):
            r[i].append(Bool(f'var_{i}_iter_{j}'))

    # initialize solver
    s = Solver()

    # initial state: states[0] is defaultly the initial state
    init_state = []
    for i in range(len(atoms)):
        if states[0][i] == "1":
            init_state.append(r[i][0])
        else:
            init_state.append(Not(r[i][0]))
    s.add(And(* init_state))


    # transitions, for example
    # trans = {"0000":["0001"], "0001":["0010","1101"], "0010":["0001"],"1101":"0000"}
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



    # property: AG (~a1 or ~a2 ... ~an) we will never reach state 11...1
    if k == 0:
        s.add(And(* [r[i][0] for i in range(len(atoms))] ))
    else:
        props = []
        for j in range(k+1):
            props.append(And(* [r[i][j] for i in range(len(atoms))] ))
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
            print(f"STEP {j}: ", end ="")
            for i in range(len(atoms)):
                if m[r[i][j]]:
                    print("1", end="")
                else:
                    print("0", end="")
            print("")



#  Atoms: a, b, c, d
#  States: 0000 (a=0, b=0, c=0, d=0), 0001, 0010, 1101, 1111
#  Initial State: 0000
#  Transitions: 0000->0001, 0001->0010, 0010->0001, 0001->1101, 1101->0000
#  Property: AG(~a or ~b or ~c or ~d) we will never reach state 1111
if __name__ == "__main__":
    k = 3
    atoms = ["a", "b"]
    states = ["00", "01", "10", "11"]
    trans = {"00":["01"], "01":["10"], "10":["11"]}

    bounded_model_check(k, atoms, states, trans)