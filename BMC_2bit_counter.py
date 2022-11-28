# Model for the 2-Bit Counter
#   States: 00, 01, 10, 11
#   Initial State: 00
#   Transitions: 00 -> 01 -> 10 -> 11

# Property: AG(~l or ~r) we will never reach state 11
#   expect the property to be satisfied within 2 steps (the SAT problem will be UNSAT)
#   expect to find counterexample at STEP (the SAT problem will be SAT)


import cvc5
from cvc5 import Kind
from cvc5.pythonic import *


def bounded_model_check(k):
    # create variables
    left = []
    right = []

    # initialize all variables
    for i in range(k + 1):
        left.append(Bool(f'l[{i}]'))
        right.append(Bool(f'r[{i}]'))

    # initialize solver
    s = Solver()

    # initial state: 00
    s.add(And(Not(left[0]), Not(right[0])))

    # transitions: 00 -> 01 -> 10 -> 11 -> 00
    for i in range(k):
        # transition for the left bit
        s.add(left[i + 1] == (left[i] == Not(right[i])))
        # transition for the right bit
        s.add(right[i + 1] == Not(right[i]))

    # property: AG (~l or ~r) we will never reach state 11
    if k == 0:
        s.add(And(left[0], right[0]))
    else:
        s.add(Or(*[And(left[i], right[i]) for i in range(k + 1)]))

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
        for i in range(k + 1):
            print(f"STEP {i}: l[{i}] = {m[left[i]]}, r[{i}] = {m[right[i]]}")

            # break if we find counterexample early
            if m[left[i]] and m[right[i]]:
                print(f"Counterexample found at STEP {i}")
                return True


def iterative_bounded_model_check(limit):
    for k in range(limit+1):
        if not bounded_model_check(k):
            continue
        else:
            return


if __name__ == "__main__":
    iterative_bounded_model_check(5)
