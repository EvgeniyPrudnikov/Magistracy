#!/bin/bash

#=============================================================
#valgrind
#=============================================================

echo valgrind_multiply_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	valgrind --tool=cachegrind --log-file="./tests/valgrind_multiply_test_$i.txt" ./cache_miss_valgrind $i 0
	echo "" >> ./tests/valgrind_multiply_test.txt;
done

echo valgrind_transpoce_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	valgrind --tool=cachegrind --log-file="./tests/valgrind_transpoce_test_$i.txt" ./cache_miss_valgrind $i 1
	echo "" >> ./tests/valgrind_transpoce_test.txt;
done
