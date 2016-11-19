import time

class Timer():

    def __init__(self):

        self.start = time.time()

    def __enter__(self):
        return self.start

    def __exit__(self, type, value, traceback):
        duration = time.time() - self.start
        print duration
        return True


def do_smf():
    time.sleep(3)

with Timer():
    do_smf()



