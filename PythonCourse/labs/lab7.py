def pythagorean_triple(n):
    res = []
    [res.append([i, j, k]) for i in xrange(3, n) for j in xrange(i, n) for k in xrange(j, n) if (i**2 + j**2 == k**2 and n > 0)]
    return res

print pythagorean_triple(200)

