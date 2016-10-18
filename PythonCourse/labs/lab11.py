class NVector:
    def __init__(self, obj):
        if isinstance(obj, list):
            self.vector = obj
            self.length = len(obj)
        else:
            self.vector = [obj]
            self.length = 1

    def __getitem__(self, item):
        return self.vector[item]

    def __str__(self):
        return str(self.vector)

    def __len__(self):
        return len(self.vector)

    def __abs__(self):
        return NVector([abs(i) for i in self.vector])

    def __add__(self, other):
        return NVector([self.vector[i] + other[i] for i, e in enumerate(self.vector)])

    def __mul__(self, other):
        return NVector([self.vector[i] * other for i, e in enumerate(self.vector)])

    def __sub__(self, other):
        return NVector([self.vector[i] - other[i] for i, e in enumerate(self.vector)])

    def __eq__(self, other):
        return all([self.vector[i] == other[i] for i, e in enumerate(self.vector)])

a = NVector([1, -2, 3, 4, 5])
b = NVector([1, -2, 3, 4, 7])

print a == b
print
