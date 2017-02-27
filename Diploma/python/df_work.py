# import main as m
from main import df

df2 = df
df2.show()
# df1.printSchema()

df2 = df2.withColumn('lol', df2['Id'] + df2['L0_S0_F0']).drop(df2['Id']).drop(df2['L0_S0_F0'])

df2.show()
