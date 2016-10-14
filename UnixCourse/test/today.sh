#!/bin/sh


foldername1=`date +%Y`
foldername2=`date +%m`
foldername3=`date +%d`
echo $foldername1
echo $foldername2
echo $foldername3
path1=`pwd`
echo $path1

path2="$path1/$foldername1 $foldername2 $foldername3"
echo "$path2"
mkdir -p "$path2"


echo "$path2"
