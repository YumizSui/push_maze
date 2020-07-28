#!/bin/sh
MAP_SIZE=$(cat $1 |head -n1|wc -m)
MINSTEP=$(expr $(cat $1 |grep min_step|awk '{print $3}') '+' 2) 
while :; do
  for (( i=1; i<$MINSTEP; i++ )); do  
    clear
    cat $1 | head -n $MAP_SIZE
    cat $1 | sed -n "$(expr $(expr $i '*' $MAP_SIZE) + 1),$(expr $(expr $i '*' $MAP_SIZE) + $MAP_SIZE)p"
    cat $1 |tail -n 4
    sleep 0.5
  done
done
