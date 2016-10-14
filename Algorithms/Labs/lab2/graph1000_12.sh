#!/bin/bash

echo "started "
for i in {1..1000}
do 

	mpiexec -n 2 /home/jonscat/Study/Algorithms/Lectures/02_web_graph/graph500-2.1.4/mpi/graph500_mpi_simple 12 16 >> test_1000_12.txt
	
	echo "">> test_1000_12.txt

done
echo "finished"
