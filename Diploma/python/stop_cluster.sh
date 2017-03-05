#!/bin/bash

cd "$SPARK_HOME"sbin/

./stop-slave.sh

sleep 3s

./stop-master.sh

