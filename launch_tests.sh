#!/bin/bash

EXEC=$1
LOGFILE=./log/${EXEC##*/}.run

if [[ ! -f $EXEC ]]
then
    echo ""
    echo "## Error : Executable $EXEC does not exist."
    echo "   Usage : $0 name_of_target"
    echo ""
    exit
fi

declare -a kernels=("base" "inv_loop" "inv_loop_onediv" "inv_loop_onediv_tiled" "inv_loop_onediv_omp")

echo ""
echo "Running performance tests for $EXEC"
echo ""

for kernel in ${kernels[@]}; do
    echo "Testing kernel :    $kernel"
    echo "perf stat -e cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,L1-dcache-store-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses $EXEC $kernel 2>&1 | tee -a $LOGFILE"
    perf stat -e cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores,L1-dcache-store-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses $EXEC $kernel 2>&1 | tee -a $LOGFILE
done
