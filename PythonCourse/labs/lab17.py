def compress(lst):
    assert len(lst) > 0, 'sequence is empty'
    el_cnt = 1
    for i in xrange(len(lst) - 1):
        if lst[i + 1] == lst[i]:
            el_cnt += 1
        else:
            yield el_cnt
            yield lst[i]
            el_cnt = 1
    yield el_cnt
    yield lst[len(lst) - 1]


def decompress(lst):
    assert len(lst) > 0, 'sequence is empty'
    assert len(lst) % 2 == 0, 'wrong sequence'
    el_cnt = 1
    for i in xrange(len(lst)):
        if i % 2 == 0:
            el_cnt = lst[i]
            assert isinstance(el_cnt, int), 'wrong sequence!'
            assert el_cnt >= 1, 'wrong sequence'
        else:
            for j in xrange(el_cnt):
                yield lst[i]


lol = ['hi', 'q', 'q', 'q', 'a', 3, 3, 3, 3, 3, 145, 'lol', 'lol']
lol1 = [1]

lol2 = list(compress(lol1))

lol3 = list(decompress(lol2))

print lol2
print lol3

print list(xrange(10,-100,-32))