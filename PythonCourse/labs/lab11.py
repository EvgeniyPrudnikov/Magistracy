class NVector:
    def __init__(self, obj):

        if isinstance(obj, list):
            self.__vector = obj[:]
            self.__length = len(obj)
        else:
            self.__vector = [obj]
            self.__length = 1

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


lol = [1, -2, 3, 4, 5]

a = NVector(lol)
b = NVector([1, -2, 3, 4, 7])

print a
print
