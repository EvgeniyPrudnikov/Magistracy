print 'seq:',
seq = list(input())

print 'a:',
a = int(input())
print 'b:',
b = int(input())


def trunc_seq(i_seq, i_a, i_b):
    return map(lambda x: i_a if x < i_a else i_b if x > i_b else x, i_seq)


print trunc_seq(seq, a, b)
