#!/bin/bash

if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
  echo "numheader: Script reads only the first line of input stream which is a tab separated column names and prints column number and column name."
  exit 0
fi


awk '{if (NR <= 1) {for(i=1;i<=NF;i++) print i"\t"$i}}' $1


# prints as many rows as there are columns in first line and adds to each column column with column number 
#awk '{if (FNR <= NF) {
#	  for(i=1;i<=NF;i++) {
#		 printf i"\t%s\t" ,$i
#		};  
#	print ""
#	}
#     }' $1
