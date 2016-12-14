def my_xrange(start, stop=None, step=None):
    assert isinstance(start, int), 'type error! an integer is required'
    assert isinstance(stop, int), 'type error! an integer is required'
    assert isinstance(step, int), 'type error! an integer is required'

    x = 0 if stop is None else start
    end = start if stop is None else stop
    x_step = 1 if step is None else step

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


print list(my_xrange(10, 0, 0))
print list(xrange(10, 0, 0))
