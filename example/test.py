import cvc5
from cvc5 import Kind
from cvc5.pythonic import *


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
    

