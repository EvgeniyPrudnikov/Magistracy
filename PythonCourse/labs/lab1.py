print 'seq:',
a = list(input())


def cum_sum(lst):
    return list(sum(lst[:i]) for i in range(len(lst) + 1))

print cum_sum(a)
