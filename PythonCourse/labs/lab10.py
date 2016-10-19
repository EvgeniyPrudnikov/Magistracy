test = [1, [2, {'a': [1, 2, 3], 'b': {'c':[1,2,3],'d':'str'}}], 4]
print test
print ''


def format_print(obj, res=[], lvl=0, et=None):

    if isinstance(obj, (str, int, float)):
        res.append(('' if len(res) > 0 and ':' in res[-1] else ' ' * lvl) + str(obj) + (',\n' if et != dict else ':'))

    elif isinstance(obj, list):

        res.append(('' if len(res) > 0 and ':' in res[-1] else ' ' * lvl) + '[' + '\n')

        for e in obj:
            format_print(e, lvl=lvl + 2, et=list)

        res.append(' ' * lvl + '],\n')

    elif isinstance(obj, dict):

        res.append(('' if len(res) > 0 and ':' in res[-1] else ' ' * lvl) + '{' + '\n')

        for e in obj:
            format_print(e, lvl=lvl + 2, et=dict)
            format_print(obj[e], lvl=lvl + 3, et=obj[e])

        res.append(' ' * lvl + '},\n')

    return (''.join(res))[:-2]


print format_print(test)