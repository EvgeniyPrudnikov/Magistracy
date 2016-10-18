class NDArray:
    def __init__(self, obj):
        self.array = [obj]

    def __str__(self):
        return str(self.array)


NArr = NDArray(1)

print NArr
