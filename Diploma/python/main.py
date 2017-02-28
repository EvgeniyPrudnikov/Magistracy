from pyspark import SparkContext
from pyspark.sql import SQLContext, Row
import numpy as np
import pandas as pd
import random
from pyspark.sql.types import *
from pyspark.sql.functions import udf
# import time as t

# t_start = t.time()

sc = SparkContext()
sqlContext = SQLContext(sc)

numeric_file = '/home/jonscat/Study/diploma_data/train_num_test.csv'
# date_file = '/home/jonscat/Study/diploma_data/train_date.csv'


def create_df(filename):
    text = sc.textFile(filename)
    header_names = list(text.first().split(','))
    header_row = text.first()

    no_header_text = text.filter(lambda row: row != header_row)

    object_rows = Row(*header_names)  # unpack list

    schema = no_header_text.map(lambda line: line.split(',')) \
        .map(lambda l: [None if i == '' else int(i) if i == 'Id' else int(float(i) * 1000) for i in l]) \
        .map(lambda line: object_rows(*line)) \

    df = sqlContext.createDataFrame(schema, samplingRatio=0.001)
    return df


df = create_df(numeric_file)

#print 'done in {0}'.format(round(t.time() - t_start, 2))
