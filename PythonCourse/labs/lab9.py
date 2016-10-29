import itertools


class plurality:
    def __init__(self, obj, n):
        self.inArr = obj[:]
        self.n = n
        self.res = sorted(list(itertools.product(*[obj for i in xrange(n)]))) # sorted right
        self.counter = 0

    def current(self):
        return self.res[self.counter]

    def next(self):
        if self.counter < len(self.res) - 1:
            self.counter += 1
        else:
            self.counter = 0
        return self.res[self.counter]


X = [1, 'a']

n = 2

a = plurality(X, n)

print a.res

print a.current()

print a.next()
print a.next()
print a.next()
print a.next()
print a.next()
print a.next()

