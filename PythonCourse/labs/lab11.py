class NVector:
    def __init__(self, obj):

        if isinstance(obj, list):
            self.__vector = list(obj)

        else:
            self.__vector = [obj]

    def __getitem__(self, item):
        return self.__vector[item]

    def __str__(self):
        return str(self.__vector)

    def __len__(self):
        return len(self.__vector)

    def __abs__(self):
        return NVector([abs(i) for i in self.__vector])

    def __add__(self, other):
        return NVector([self.__vector[i] + other[i] for i, e in enumerate(self.__vector)])

    def __mul__(self, other):
        if isinstance(other, (int, float)):
            return NVector([self.__vector[i] * other for i, e in enumerate(self.__vector)])
        else:
            return sum([self.__vector[i] * other[i] for i, e in enumerate(self.__vector)])

    def __sub__(self, other):
        return NVector([self.__vector[i] - other[i] for i, e in enumerate(self.__vector)])

    def __eq__(self, other):
        return all([self.__vector[i] == other[i] for i, e in enumerate(self.__vector)])

    def __pow__(self, power, modulo=None):
        return NVector([self.__vector[i] ** power for i, e in enumerate(self.__vector)])

    def scalar_square(self):
        return sum([self.__vector[i] * self.__vector[i] for i, e in enumerate(self.__vector)])

    def scalar_len(self):
        return self.scalar_square() ** 0.5

    def distance(self, other):
        return (sum([(self.__vector[i] - other[i]) ** 2 for i, e in enumerate(self.__vector)]))**0.5

    def phi(self, other):
        other.__class__ = NVector
        return self.distance(other) / self.scalar_len() * other.scalar_len()

a = NVector([1, -2, 3, 4, 7])

b = NVector([1, -2, 3, 4, 7])

print 'a =',
print a
print 'b =',
print b
print ''
print 'a + b =',
print a + b
print 'a * 4 =',
print a * 4
print 'a ** 3 =',
print a ** 3
print 'a * b =',
print a * b
print 'a == b =',
print a == b
print 'scalar square =',
print a.scalar_square()
print 'scalar length =',
print a.scalar_len()
print 'distance =',
print a.distance(b)
print 'angle =',
print a.phi(b)


