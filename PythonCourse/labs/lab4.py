s = '_____The___Day_That_Never_Comes___'
# s = '___1___2_3_4_5____________'
# res = '_____Comes___Never_That_Day_The___'
print s


def rev(i_s, sep):

    words = i_s.split(sep)
    w_ind = list([x for x, e in enumerate(words) if e != ''])

    '''
    print w_ind

    for i in range(len(s.split(sep))):
        if s.split(sep)[i] != '':
            print i,
            print words[i],
            print w_ind.index(i),
            print w_ind[::-1].index(i),
            print w_ind[w_ind[::-1].index(i)],
            print words[w_ind[w_ind[::-1].index(i)]]
    '''
    return sep.join(list(words[w_ind[w_ind[::-1].index(i)]] if e != '' else '' for i, e in enumerate(words)))

print rev(s, '_')
