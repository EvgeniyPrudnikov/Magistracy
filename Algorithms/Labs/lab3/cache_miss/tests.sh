#!/bin/bash

#=============================================================
# my analyzer 
#=============================================================

echo lru_multiply_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	./cache_miss $i 0 0 >> ./tests/lru_multiply_test.txt;
	echo "" >> ./tests/lru_multiply_test.txt;
done

echo random_multiply_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	./cache_miss $i 1 0 >> ./tests/random_multiply_test.txt;
	echo "" >> ./tests/random_multiply_test.txt;
done

echo lru_transpoce_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	./cache_miss $i 0 1 >> ./tests/lru_transpoce_test.txt;
	echo "" >> ./tests/lru_transpoce_test.txt;
done

echo random_transpoce_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	./cache_miss $i 1 1 >> ./tests/random_transpoce_test.txt;
	echo "" >> ./tests/random_transpoce_test.txt;
done

#=============================================================
#valgrind
#=============================================================

echo valgrind_multiply_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	valgrind --tool=cachegrind --log-file="./tests/valgrind_multiply_test.txt" ./cache_miss_valgrind $i 0
	echo "" >> ./tests/valgrind_multiply_test.txt;
done

echo valgrind_transpoce_test
for i in 1023 1024 1025 1040 1041 1050 1100
do
	echo "n = $i";
	valgrind --tool=cachegrind --log-file="./tests/valgrind_transpoce_test.txt" ./cache_miss_valgrind $i 1
	echo "" >> ./tests/valgrind_transpoce_test.txt;
done
