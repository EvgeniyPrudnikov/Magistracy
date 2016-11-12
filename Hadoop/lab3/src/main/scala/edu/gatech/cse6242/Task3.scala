package edu.gatech.cse6242

import org.apache.hadoop.hbase.HBaseConfiguration
import org.apache.hadoop.hbase.client.Put
import org.apache.hadoop.hbase.client.Result
import org.apache.hadoop.hbase.io.ImmutableBytesWritable
import org.apache.hadoop.hbase.mapred.TableOutputFormat
import org.apache.hadoop.hbase.mapreduce.TableInputFormat
import org.apache.hadoop.hbase.util.Bytes
import org.apache.hadoop.mapred.JobConf
import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.rdd.PairRDDFunctions
import org.apache.spark.SparkContext._
import org.apache.spark.rdd.RDD
import org.apache.spark.util.StatCounter

object Task3 {

  def main(args: Array[String]) {

    val tableName = args(0)
    val colFamDataBytes = Bytes.toBytes("edge")

    val sparkConf = new SparkConf().setAppName("Task3")
    val sc = new SparkContext(sparkConf)    

    val conf = HBaseConfiguration.create()
    conf.set(TableInputFormat.INPUT_TABLE, tableName)

    conf.set(TableInputFormat.SCAN_COLUMNS, "edge:target_node_id edge:weight")

    // Load an RDD of (ImmutableBytesWritable, Result) tuples from the table
    val hBaseRDD = sc.newAPIHadoopRDD(conf, classOf[TableInputFormat],
      classOf[ImmutableBytesWritable],
      classOf[Result])
    // transform (ImmutableBytesWritable, Result) tuples into an RDD of Result’s
    val resultRDD = hBaseRDD.map(tuple => tuple._2)

    val keyValueRDD = resultRDD.map(result => (Bytes.toString(result.getValue(colFamDataBytes, Bytes.toBytes("target_node_id"))),
                                                (Bytes.toString(result.getValue(colFamDataBytes, Bytes.toBytes("weight")))).toInt
                                               )
                                    )
    val reducedRDD = keyValueRDD.reduceByKey{(x, y) => x + y}

    reducedRDD.map(x => x._1 + "\t" + x._2).saveAsTextFile("hdfs://localhost:8020" + args(1))

  }
}


