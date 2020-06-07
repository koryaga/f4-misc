#!/bin/bash
# Testing script for F4 environment

for i in test_*.ram; do
  echo -n "Running test $i "
  #extract test result
  RES=$(echo $i| awk '{match($0,/_([0-9]+).ram$/,arr); print arr[1];}')
  
  # run test with 1 sec timeout. Get printed result
  ACTUAL=$(timeout 1 bash -c " cat $i | ./f4cli | tail -n1 " | awk -e '{ match($0,/A=([0-9]+)/,arr);
print arr[1];}')
  echo -n "Expected $RES, got $ACTUAL "
  [ "$RES" = "$ACTUAL" ]&& echo OK|| echo Fail

  
done

