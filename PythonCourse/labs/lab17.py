def compress(lst):
    el_cnt = 1
    res = []
    for i in xrange(len(lst) - 1):
        if lst[i + 1] == lst[i]:
            el_cnt += 1
        else:
            res.append(el_cnt)
            res.append(lst[i])
            el_cnt = 1

    res.append(el_cnt)
    res.append(lst[len(lst) - 1])

    return res


def decompress(lst):
    el_cnt = 1
    res = []
    for i in xrange(len(lst)):
        if i % 2 == 0:
            el_cnt = lst[i]
            assert isinstance(el_cnt, int), 'Wrong sequence!'
        else:
            for j in xrange(el_cnt):
                res.append(lst[i])

    return res


lol = ["hi", 'q', 'q', 'q', [1, 2], 3, 3, 6, 3, 3, 23, 'lol', 'lol', 'lol']

lol2 = compress(lol)

lol3 = decompress(lol2)

print lol
print lol2
print lol3
