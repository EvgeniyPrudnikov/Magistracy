def my_xrange(start, stop=None, step=None):
    x = 0 if stop is None else start
    end = start if stop is None else stop
    x_step = 1 if step is None else step
    while x < end:
        yield x
        x += x_step


print list(my_xrange(10, 15, 1))
print list(xrange(10, 15, 1))

print type(my_xrange)
print type(xrange)
