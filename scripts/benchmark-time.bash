#!/bin/bash

TEST_SET=(1 2 4 8 16)
TEST_DATA_NUM=10000

for i in ${TEST_SET[@]}; do ./sort $i ${TEST_DATA_NUM}; done
