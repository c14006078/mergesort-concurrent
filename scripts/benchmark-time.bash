#!/bin/bash

. benchmark.cfg

for i in ${TEST_SET[@]}; do ./sort $i ${TEST_DATA_NUM}; done
