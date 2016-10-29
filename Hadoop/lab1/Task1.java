package edu.gatech.cse6242;

import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class Task1 {
	
public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
	   private IntWritable weight;
	   private Text target_vert = new Text();
	
	   public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
	    String line = value.toString();
	    StringTokenizer tokenizer = new StringTokenizer(line,"\n");

	    while (tokenizer.hasMoreTokens()) {
         String tkn = tokenizer.nextToken();
	     target_vert.set(tkn.split("\t")[1]);
         weight = new IntWritable(Integer.parseInt(tkn.split("\t")[2]));
	     output.collect(target_vert, weight);
	    }
	   }
	  }
	
	  public static class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {
	   public void reduce(Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
	    int sum = 0;
	    while (values.hasNext()) {
	     sum += values.next().get();
	    }
	    output.collect(key, new IntWritable(sum));
	   }
	  }

  public static void main(String[] args) throws Exception {
    JobConf conf = new JobConf(Task1.class);
    conf.setJobName("Task1");

    /* TODO: Needs to be implemented */
     conf.setOutputKeyClass(Text.class);
	   conf.setOutputValueClass(IntWritable.class);
	
	   conf.setMapperClass(Map.class);
	   conf.setCombinerClass(Reduce.class);
	   conf.setReducerClass(Reduce.class);
	
	   conf.setInputFormat(TextInputFormat.class);
	   conf.setOutputFormat(TextOutputFormat.class);


    FileInputFormat.setInputPaths(conf, new Path(args[0]));
    FileOutputFormat.setOutputPath(conf, new Path(args[1]));

    JobClient.runJob(conf);
  }
}

