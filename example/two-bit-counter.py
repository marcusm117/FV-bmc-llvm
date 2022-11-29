import cvc5
from cvc5 import Kind
from cvc5.pythonic import *

# 4 states: (not l, not r), (l, not r), (not l, r), (l, r)
# Initial state I0: not l and not r
# Transition Relation R: l' = (l != r) and r' = not r
#
# Property to check: AG(not l or not r)
#   expect to be violated within 3 steps (SAT)
#   within 2 steps, this will be UNSAT
#
# omega(k) is the conjunction of the initial state, the transition relation
# unrolled k times (as a conjunction), and the property at all steps
# (disjunction of the negation)
# 
# omega(2)

if __name__ == "__main__":
    # create variables
    x, y = Reals('x y')
    a, b = Ints('a b')

    # initialzie solver
    s = Solver()

    # add constraints
    s.add(0 < x, 0 < y, x + y < 1, x <= y)

    assert sat == s.check()
    m = s.model()

    # print out results
    print('x:', m[x])
    print('y:', m[y])
    print('x - y:', m[x - y])
    

