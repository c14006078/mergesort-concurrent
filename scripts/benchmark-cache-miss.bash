#!/bin/bash

. benchmark.cfg

for i in ${TEST_SET[@]}; do perf stat -e cache-misses,cache-references,instructions,cycles ./sort $i ${TEST_DATA_NUM}; done
