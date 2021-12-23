#!/bin/bash

##########################################################
# launch_tests.sh
#
# Launch a battery of performance tests on the specified
# executables and for the specified kernels using perf.
#
# Author : Gabriel Suau
#
##########################################################

if [[ $# -lt 1 ]]
then
    echo ""
    echo "## Error : No executable provided."
    echo "   Usage : $0 exec_1 exec_2 ... exec_n"
    echo ""
    exit
fi

declare -a kernels=("base" "inv_loop" "inv_loop_onediv" "inv_loop_unroll2" "inv_loop_tiled" "inv_loop_omp")

for EXEC in $@; do

    if [[ ! -x $EXEC ]]
    then
        echo ""
        echo "## Error : File $EXEC is not an executable. Moving to next file in list..."
        echo ""
        continue
    fi

    LOGFILE=./log/${EXEC##*/}.run

    echo ""
    echo "--------------------------------------------------"
    echo "Running performance tests for $EXEC"
    echo "--------------------------------------------------"
    echo ""

    for KERNEL in ${kernels[@]}; do
        echo "Testing kernel :    $KERNEL"
        echo "perf stat -e cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,L1-dcache-store-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses $EXEC $KERNEL 2>&1 | tee -a $LOGFILE"
        perf stat -e cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,L1-dcache-store-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses $EXEC $KERNEL | tee -a $LOGFILE
    done

done
