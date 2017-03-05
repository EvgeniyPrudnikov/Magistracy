#!/bin/bash

cd "$SPARK_HOME"sbin/
./start-master.sh

sleep 3s

./start-slave.sh spark://cathome:7077

sleep 4s

google-chrome http://localhost:8080




