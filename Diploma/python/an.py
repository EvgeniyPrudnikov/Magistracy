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

#TODO: add corr matrix code
#TODO: add analysis code

