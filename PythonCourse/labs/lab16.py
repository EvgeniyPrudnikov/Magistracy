def my_xrange(start, stop=None, step=None):

    x = 0 if stop is None else start
    end = start if stop is None else stop
    x_step = 1 if step is None else step

    assert isinstance(x, int), 'type error! an integer is required'
    assert isinstance(end, int), 'type error! an integer is required'
    assert isinstance(x_step, int), 'type error! an integer is required'

    if x < end:
        if x_step <= 0: return
        while x < end:
            yield x
            x += x_step
    else:
        if x_step >= 0: return
        while x > end:
            yield x
            x += x_step


print list(my_xrange(10, -100, -32))

#print list(xrange(-1, -10, -1))
