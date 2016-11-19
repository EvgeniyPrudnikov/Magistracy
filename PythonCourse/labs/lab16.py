def my_xrange(start, stop=None, step=None):
    x = 0
    x_step = 1 if step is None else step
    while x <= start:
        yield x
        x += x_step


print list(my_xrange(10, step=3))
print list(my_xrange(10, step=3))
