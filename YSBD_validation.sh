#!/bin/bash

if [ $# -lt 4 ]
then
    echo -e "\033[1;31mError:\033[0m Missing arguments\n"
elif [ $2 -lt 0 ]
then
    echo -e "\033[1;31mError:\033[0m Lower Bound should be a non-negative number\n"
else
    step=${5:-1}

    for i in $(seq $2 $step $3);
    do
        echo -e "\033[1;33mEntries:" $i"\033[0m"
        ./$1 $i $4 | grep "Checkpoint Result";
        echo -e "\n";
    done
fi
