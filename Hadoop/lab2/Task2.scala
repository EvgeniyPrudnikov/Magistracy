package edu.gatech.cse6242

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf

object Task2 {

  def main(args: Array[String]) {

    val sc = new SparkContext(new SparkConf().setAppName("Task2"))

    // read the file
    val file = sc.textFile("hdfs://localhost:8020" + args(0))

    // Split up into tableLines.
    val tableLine = file.flatMap(line => line.split("\n"))
    // Transform into verticle and weight.
    val vertWeights = tableLine.map(tableLine => (tableLine.split("\t")(1), tableLine.split("\t")(2).toInt)).reduceByKey{case (accumulatedValue, currentValue) => accumulatedValue + currentValue}

    // store output on given HDFS path.
    vertWeights.map(x => x._1 + "\t" + x._2).saveAsTextFile("hdfs://localhost:8020" + args(1))
  }
}