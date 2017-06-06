#Enviroment preparation

from pyspark import SparkContext, SparkConf
from pyspark.sql import SQLContext, Row
from pyspark.sql.types import *
import pyspark.sql.functions as F
import numpy as np
import pandas as pd
import random
import time as t
import os
import re
import gc
from subprocess import Popen
import matplotlib.pyplot as plt
#% matplotlib inline



def ControlSparkCluster(action):
    if action == 'start':
        Popen('./start_cluster.sh', shell=True)
        t.sleep(10)
        print 'cluster started'
    elif action == 'stop':
        Popen('./stop_cluster.sh', shell=True)
        sc.stop()
        t.sleep(3)
        print 'cluster stopped'
    else:
        raise Exception('Unsupported command!')

ControlSparkCluster('stop')
ControlSparkCluster('start')
# cluster started

conf = SparkConf().setMaster("spark://cathome:7077").setAppName("dpl") \
    .setAll((['spark.executor.cores', '4']
             , ['spark.driver.cores', '4']
             , ['spark.driver.memory', '1g']
             , ['spark.driver.maxResultSize', '4g']
             , ['spark.executor.memory', '2g']
             , ['spark.python.worker.memory', '1g']
             , ['spark.default.parallelism', '4']
             ))

sc = SparkContext(conf=conf)
sqlContext = SQLContext(sc)
#Data preparation

dir_name = '../../../diploma_data'

tr_num_file_name = 'train_numeric.csv'
tr_date_file_name = 'train_date.csv'
tr_cat_file_name = 'train_categorical.csv'

tr_num_data_file = '{0}/{1}'.format(dir_name, tr_num_file_name)
tr_date_data_file = '{0}/{1}'.format(dir_name, tr_date_file_name)
tr_cat_data_file = '{0}/{1}'.format(dir_name, tr_cat_file_name)

#%% time
df_num = sqlContext.read.csv(tr_num_data_file, sep=',', header=True, inferSchema=True)
df_date = sqlContext.read.csv(tr_date_data_file, sep=',', header=True, inferSchema=True)
df_cat = sqlContext.read.csv(tr_cat_data_file, sep=',', header=True, inferSchema=True)
#Data analisys / Data transformation

df_num2 = df_num
df_date2 = df_date
df_cat2 = df_cat

num_sample_file = '{0}/{1}'.format(dir_name, 'num_sample.parquet')
date_sample_file = '{0}/{1}'.format(dir_name, 'date_sample.parquet')
cat_sample_file = '{0}/{1}'.format(dir_name, 'cat_sample.parquet')

# df_num.sample(False, 0.02, seed=112).repartition(1024).write.parquet(num_sample_file)
# df_date.sample(False, 0.02, seed=112).repartition(1024).write.parquet(date_sample_file)
# df_cat.sample(False, 0.02, seed=112).repartition(4096).write.parquet(cat_sample_file)

#%% time
df_num_smpl = sqlContext.read.parquet(num_sample_file)
df_date_smpl = sqlContext.read.parquet(date_sample_file)

# CPU times: user 4 ms, sys: 4 ms, total: 8 ms
# Wall time: 1.07 s

df_num2.limit(10).toPandas()

df_date2.limit(10).toPandas()

df_cat2.limit(10).toPandas()

print 'numeric dataset row count: ', df_num2.count()
print 'date dataset row count: ', df_date2.count()
print 'categorical dataset row count: ', df_cat2.count()

#numeric dataset row count:  1183747
#date dataset row count:  1183747
#categorical dataset row count:  1183747

# df_num2.printSchema()
# df_date2.printSchema()
# df_cat2.printSchema()


df_num2.describe(df_num2.columns[1:5]).show()
#+-------+--------------------+--------------------+--------------------+--------------------+
#| summary | L0_S0_F0 | L0_S0_F2 | L0_S0_F4 | L0_S0_F6 |
#+-------+--------------------+--------------------+--------------------+--------------------+
#| count | 673862 | 673862 | 673862 | 673862 |
#| mean | 5.456013249000960... | 9.128575286932377E-5 | 4.005271108873819... | 1.383517693495106... |
#| stddev | 0.08021442344615412 | 0.09279893101577752 | 0.2115196284424472 | 0.2116351123015797 |
#| min | -0.565 | -0.616 | -0.415 | -0.416 |
#| max | 0.278 | 0.302 | 0.585 | 0.584 |
#+-------+--------------------+--------------------+--------------------+--------------------+

df_date2.describe(df_date2.columns[1:5]).show()
#+-------+------------------+------------------+------------------+------------------+
#| summary | L0_S0_D1 | L0_S0_D3 | L0_S0_D5 | L0_S0_D7 |
#+-------+------------------+------------------+------------------+------------------+
#| count | 673862 | 673862 | 673862 | 673862 |
#| mean | 882.2290677468101 | 882.2290677468101 | 882.2290677468101 | 882.2290677468101 |
#| stddev | 506.72491575187627 | 506.72491575187627 | 506.72491575187627 | 506.72491575187627 |
#| min | 0.0 | 0.0 | 0.0 | 0.0 |
#| max | 1713.71 | 1713.71 | 1713.71 | 1713.71 |
#+-------+------------------+------------------+------------------+------------------+

df_cat2.describe(df_cat2.columns[1:5]).show()
#+-------+---------+---------+---------+---------+
#| summary | L0_S1_F25 | L0_S1_F27 | L0_S1_F29 | L0_S1_F31 |
#+-------+---------+---------+---------+---------+
#| count | 25 | 25 | 27 | 27 |
#| mean | null | null | null | null |
#| stddev | null | null | null | null |
#| min | T1 | T9 | T1 | T24 |
#| max | T1 | T9 | T1 | T9 |
#+-------+---------+---------+---------+---------+

cols_num = [(c.split('_')) for c in df_num2.columns]
cols_num_df = pd.DataFrame(cols_num[1:-1], columns=['line', 'station', 'feature'])
cols_num_df
cols_num_df_g = cols_num_df.groupby(['line', 'station'], sort=False).count()
cols_num_df_g

cols_date = [(c.split('_')) for c in df_date2.columns]
cols_date_df = pd.DataFrame(cols_date[1:], columns=['line', 'station', 'feature'])
cols_date_df
cols_date_df_g = cols_date_df.groupby(['line', 'station'], sort=False).count()
cols_date_df_g

cols_cat = [(c.split('_')) for c in df_cat2.columns]
cols_cat_df = pd.DataFrame(cols_cat[1:], columns=['line', 'station', 'feature'])
cols_cat_df
cols_cat_df_g = cols_cat_df.groupby(['line', 'station'], sort=False).count()
cols_cat_df_g


def plt_hist(counts_by_col_d):
    hist_v = map(float, [v for k, v in sorted(counts_by_col_d.items())])
    elems = list(xrange(0, len(hist_v)))
    plt.figure(figsize=(25, 6))
    plt.ylabel('proc')
    plt.xlabel('col id')
    plt.grid(True)
    plt.xlim([-10, len(elems) + 10])
    width = 1 / 1
    # plt.xticks(elems, row_fileds)
    plt.bar(elems, hist_v, width, color="blue")


def plt_pipe(counts_1_all_l):
    labels = 'Response:1', 'Response:0'
    sizes = (counts_1_all_l['count_1'], counts_1_all_l['count_all'])
    explode = (0.2, 0)

    fig1, ax1 = plt.subplots()
    ax1.pie(sizes, explode=explode, labels=labels, autopct='%1.1f%%',
            shadow=True, startangle=90)
    ax1.axis('equal')


counts_1_all_l = df_num2.select(F.sum(F.when(df_num2['Response'] == 1, 1).otherwise(0)).alias('count_1'),
                                F.count('*').alias('count_all')).collect()

plt_pipe(counts_1_all_l[0].asDict())

counts_by_col = df_num2.select(*[(F.count(c) / F.count("*")).alias(c) for c in df_num2.columns])

counts_by_col.limit(1).toPandas()

counts_by_col_l = counts_by_col.collect()

plt_hist(counts_by_col_l[0].asDict())

col_with_resp = df_num2.filter(df_num2.Response == 1).select(
    *[(F.count(c) / F.count("*")).alias(c) for c in df_num2.columns])

col_with_resp.limit(100).toPandas()

col_with_resp_l = col_with_resp.collect()

plt_hist(col_with_resp_l[0].asDict())

counts_by_col3 = df_date2.select(*[(F.count(c) / F.count("*")).alias(c) for c in df_date2.columns])

counts_by_col3.limit(1).toPandas()

counts_by_col_l3 = counts_by_col3.collect()

plt_hist(counts_by_col_l3[0].asDict())

counts_by_col4 = df_cat2.select(*[(F.count(c) / F.count("*")).alias(c) for c in df_cat2.columns])

counts_by_col_l4 = counts_by_col4.collect()

plt_hist(counts_by_col_l4[0].asDict())



pdf_date = df_date_smpl.toPandas()

cols_date = [(c.split('_')) for c in pdf_date.columns]
cols_date_df = pd.DataFrame(cols_date[1:], columns=['line','station','feature'])
cols_date_df['full_name'] = cols_date_df.line + '_' + cols_date_df.station + '_' + cols_date_df.feature
cols_date_df = cols_date_df.drop('line',1)
for i in list(cols_date_df.station.unique()):
    station_features = list(cols_date_df.loc[cols_date_df['station'] == i].full_name.values)
    pdf_date[station_features].corr(method='pearson')

#    L0_S0_D1    L0_S0_D3    L0_S0_D5    L0_S0_D7    L0_S0_D9    L0_S0_D11   L0_S0_D13   L0_S0_D15   L0_S0_D17   L0_S0_D19   L0_S0_D21   L0_S0_D23
#L0_S0_D1    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D3    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D5    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D7    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D9    1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D11   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D13   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D15   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D17   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D19   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D21   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S0_D23   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#Out[79]:
#L0_S1_D26   L0_S1_D30
#L0_S1_D26   1.0 1.0
#L0_S1_D30   1.0 1.0
#Out[79]:
#L0_S2_D34   L0_S2_D38   L0_S2_D42   L0_S2_D46   L0_S2_D50   L0_S2_D54   L0_S2_D58   L0_S2_D62   L0_S2_D66
#L0_S2_D34   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D38   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D42   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D46   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D50   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D54   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D58   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D62   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
#L0_S2_D66   1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
# ....

# date df
pdf_num = df_mun_smpl.toPandas()

cols_date = [(c.split('_')) for c in pdf_num.columns]
cols_date_df = pd.DataFrame(cols_date[1:], columns=['line','station','feature'])
cols_date_df['full_name'] = cols_date_df.line + '_' + cols_date_df.station + '_' + cols_date_df.feature
cols_date_df = cols_date_df.drop('line',1)
for i in list(cols_date_df.station.unique()):
    station_features = list(cols_date_df.loc[cols_date_df['station'] == i].full_name.values)
    pdf_num[station_features].corr(method='pearson')

# num df 
useful_values = []
cols_date = [(c.split('_')) for c in pdf_num.columns]
cols_date_df = pd.DataFrame(cols_date[1:], columns=['line','station','feature'])
cols_date_df['full_name'] = cols_date_df.line + '_' + cols_date_df.station + '_' + cols_date_df.feature
cols_date_df = cols_date_df.drop('line',1)
for i in list(cols_date_df.station.unique()):
    station_features = list(cols_date_df.loc[cols_date_df['station'] == i].full_name.values)
    corr_df = pd.DataFrame(pdf_num[station_features].corr(method='pearson'))
    corr_df
    try:
        min_value = abs(corr_df.loc[:, station_features]).min(axis=1).idxmax(axis=0)
    except:
        min_value = None
    if min_value:
        useful_values.append(min_value)
useful_values


#    L0_S0_F0    L0_S0_F2    L0_S0_F4    L0_S0_F6    L0_S0_F8    L0_S0_F10   L0_S0_F12   L0_S0_F14   L0_S0_F16   L0_S0_F18   L0_S0_F20   L0_S0_F22
#L0_S0_F0    1.000000    0.889585    -0.024860   -0.023693   -0.021955   -0.016040   -0.778816   -0.699263   0.151366    0.206989    0.153013    0.153011
#L0_S0_F2    0.889585    1.000000    -0.023703   -0.027373   -0.042980   0.012173    -0.689760   -0.778402   0.223214    0.176426    0.199401    0.199399
#L0_S0_F4    -0.024860   -0.023703   1.000000    0.992738    -0.028634   0.049657    0.551105    0.552818    0.056977    0.046345    -0.013667   -0.013668
#L0_S0_F6    -0.023693   -0.027373   0.992738    1.000000    -0.029508   0.049524    0.554599    0.551547    0.057959    0.044261    -0.012808   -0.012810
#L0_S0_F8    -0.021955   -0.042980   -0.028634   -0.029508   1.000000    0.516884    -0.038238   -0.021610   0.042460    0.080503    -0.050112   -0.050098
#L0_S0_F10   -0.016040   0.012173    0.049657    0.049524    0.516884    1.000000    0.038773    0.016819    0.220418    0.270935    -0.000579   -0.000578
#L0_S0_F12   -0.778816   -0.689760   0.551105    0.554599    -0.038238   0.038773    1.000000    0.914613    -0.098344   -0.158758   -0.109484   -0.109484
#L0_S0_F14   -0.699263   -0.778402   0.552818    0.551547    -0.021610   0.016819    0.914613    1.000000    -0.161912   -0.138369   -0.140083   -0.140083
#L0_S0_F16   0.151366    0.223214    0.056977    0.057959    0.042460    0.220418    -0.098344   -0.161912   1.000000    0.607189    0.018838    0.018839
#L0_S0_F18   0.206989    0.176426    0.046345    0.044261    0.080503    0.270935    -0.158758   -0.138369   0.607189    1.000000    0.023059    0.023059
#L0_S0_F20   0.153013    0.199401    -0.013667   -0.012808   -0.050112   -0.000579   -0.109484   -0.140083   0.018838    0.023059    1.000000    1.000000
#L0_S0_F22   0.153011    0.199399    -0.013668   -0.012810   -0.050098   -0.000578   -0.109484   -0.140083   0.018839    0.023059    1.000000    1.000000
#Out[106]:
#L0_S1_F24   L0_S1_F28
##L0_S1_F24   1.00000 -0.26028
#L0_S1_F28   -0.26028    1.00000
#Out[106]:
#L0_S2_F32   L0_S2_F36   L0_S2_F40   L0_S2_F44   L0_S2_F48   L0_S2_F52   L0_S2_F56   L0_S2_F60   L0_S2_F64
#L0_S2_F32   1.000000    -0.336864   0.755147    -0.146511   0.294490    NaN 0.694501    0.032475    -0.114641
#L0_S2_F36   -0.336864   1.000000    0.022725    0.017860    -0.005609   NaN 0.039810    -0.016953   0.017920
#L0_S2_F40   0.755147    0.022725    1.000000    -0.600806   0.168338    NaN 0.595915    0.019304    -0.276755
#L0_S2_F44   -0.146511   0.017860    -0.600806   1.000000    0.065475    NaN -0.102562   -0.023918   0.187910
#L0_S2_F48   0.294490    -0.005609   0.168338    0.065475    1.000000    NaN 0.211209    0.058574    0.005544
#L0_S2_F52   NaN NaN NaN NaN NaN NaN NaN NaN NaN
#L0_S2_F56   0.694501    0.039810    0.595915    -0.102562   0.211209    NaN 1.000000    -0.584736   -0.020423
#L0_S2_F60   0.032475    -0.016953   0.019304    -0.023918   0.058574    NaN -0.584736   1.000000    -0.002484
#L0_S2_F64   -0.114641   0.017920    -0.276755   0.187910    0.005544    NaN -0.020423   -0.002484   1.000000