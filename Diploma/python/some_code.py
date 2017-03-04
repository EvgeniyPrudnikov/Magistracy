    schema = no_header_text.map(lambda line: line.split(','))\
        #.map(lambda l: l[0:3] + [l[-1]])\
        #.map(lambda l: [None if e == '' \
        #                 else int(e) if i == 0 or i == len(l) - 1\
        #                 else int(float(e)*1000) for i,e in enumerate(l)])\
        .map(lambda l: object_rows(*l))


def mapper(row,col_name,col_value):
    data = row.asDict()
    data[col_name] = col_value
    return Row(**data)
