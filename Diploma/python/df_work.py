# import main as m
from main import *

df2 = df
# df2 = df2.withColumn('lol', df2['Id'] + df2['L0_S0_F0']).drop(df2['Id']).drop(df2['L0_S0_F0'])
df2.show()

np_arr = np.array([random.randint(0, 99999) for i in xrange(50000)])

print np_arr

df2 = df2.withColumn('rand vector', np_arr)

df2.show()

def lol():
    return np.array([random.randint(0, 99999) for i in xrange(50000)])

my_udf = udf(lol, ArrayType(IntegerType()))

df2 = df2.withColumn('rand_vector', my_udf())

df2.show()
