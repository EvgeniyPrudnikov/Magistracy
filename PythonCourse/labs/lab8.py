
def prime_numbers(n):
    pr = [2]
    [pr.append(i) for i in xrange(3, n + 1) if not [x for x in pr if i % x == 0]]
    return pr


print prime_numbers(100);
