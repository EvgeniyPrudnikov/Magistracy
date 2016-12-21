def f(x):
    if x > 4:
        return True
    else:
        return False


class SeqWithFilter:
    def __init__(self, iter_obj, filter_func):
        self.lst_obj = iter_obj
        self.filter_fnct = filter_func

    def __iter__(self):
        for i in self.lst_obj:
            if self.filter_fnct(i):
                yield i


lol1 = [1, 2, 3, 4, 5, 6, 7, 8]
lol2 = ['a', 'b', 'c']

s = SeqWithFilter(lol2, lambda x: x > 'a')

lol2 = list([j for j in s])

print lol2
