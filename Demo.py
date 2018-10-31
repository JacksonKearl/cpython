import time

def timing(f):
    def wrap(*args):
        time1 = time.time()
        ret = f(*args)
        time2 = time.time()
        print('{:s} took {:.3f} ms'.format(f.__name__, (time2-time1)*1000.0))

        return ret
    return wrap


@timing
def buildingAString():
    "increasingly large memory allocations, a good coalese would probably do you well here"
    s = ""
    for i in range(10000):
        s = s + "_" + str(i)

@timing
def constructingObjects():
    "a ton of smaller allocations"
    class Node:
        def __init__(self, next, data):
            self.next = next
            self.data = data

    l = None
    for i in range(100000):
        l = Node(l, i)

def doingMath():
    "even more even smaller allocations"
    start = time.time()
    elapsed = lambda: (time.time() - start) * 1000

    i = 1
    while elapsed() < 1000:
        _i = i
        while _i != 1:
            if _i % 2 == 0: _i = _i // 2
            else: _i = 3*_i + 1
        i = i + 1
    print("Proved Collatz for i = (1..." + str(i) + ") in just one second!")


print("Hello from a Python that allocated this very string using *your* code. Go you!")
buildingAString()
constructingObjects()
doingMath()