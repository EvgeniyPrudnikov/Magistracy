def show():
    msgt = '{}'.format(type(i))
    msgi = 'id={:09x}'.format(id(i))
    msgv = '{}'.format(str(i))
    try:
        msgh = 'hash = {:012d}'.format(hash(i))
    except TypeError:
        msgh = 'non hash type'
    print('i - is : {:23} |{} |{} ==> {}'.format(msgt, msgi, msgh, msgv))


i = 1; show()
i = 1.5e-2; show()
i = complex(3.0, 5.5); show()
i = "UNICODE string"; show()
i = [1, 2, 3]; show()
i = [3 * x for x in range(3)]; show()
i = [[x, x ** 2] for x in range(3)]; show()
i = (1, 2, 3); show()
i = {1, 2, 3}; show()
i = {1: "one", 2: "two", 3: "three"}; show()
i = lambda x: "lambda"; show()
i = compile('lambda x: "lol"', '', 'eval'); show()
i = iter('12345'); show()


class own1:
    def __init__(self, id):
        self.id = id


i = own1(987); show()
i = own1; show()


class own2:
    def __init__(self, id):
        self.id = id

    def __hash__(self):
        return None


i = own2(987);
show()
i = own2;
show()
