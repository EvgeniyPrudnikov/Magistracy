from pyspark import SparkContext
from pyspark.sql import SQLContext, Row
from pyspark.sql.types import *

sc = SparkContext()
sqlContext = SQLContext(sc)

numeric_file = '/home/jonscat/Study/diploma_data/test_numeric.csv'


def create_df(filename):
    text = sc.textFile(filename)
    header_names = list(text.first().split(','))
    header_row = text.first()

    no_header_text = text.filter(lambda row: row != header_row)

    object_rows = Row(*header_names)  # unpack list

    schema = no_header_text.map(lambda line: line.split(',')).map(lambda line: object_rows(*line))

    df = sqlContext.createDataFrame(schema)
    return df


df1 = create_df(numeric_file)

df1.show()
