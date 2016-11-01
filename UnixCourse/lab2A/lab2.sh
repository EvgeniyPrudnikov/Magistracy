#!/bin/bash

if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
  echo "numheader: Script reads only the first line of input stream which is a tab separated column names and prints column number and column name."
  exit 0
fi


awk '{{for(i=1;i<=NF;i++) print i"\t"$i} exit}' "$1"
