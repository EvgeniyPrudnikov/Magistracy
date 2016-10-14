number = 12
print 'n = ' + str(number)


def fact(n):

    res = []
    x = 2
    while x * x <= n:
        if n % x > 0:
            x += 1
        else:
            n //= x
            res.append(x)

    if n > 1:
        res.append(n)

    return list([i, res.count(i)] for i in set(res))

print fact(number)
