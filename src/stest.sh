#!/bin/sh

if [ -z $1 ] || [ -z $2 ]; then
    echo 'usage: sbatch stest.sh <xmlsize (e.g. 3G-> /mnt/hostfs/bakadi/test3G.xml)> <num_threads (e.g. 240)>';
    exit
fi

#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./test.mic -a "/mnt/hostfs/bakadi/test$1.xml /mnt/hostfs/bakadi/xmark.tokens /mnt/hostfs/bakadi/xmark.mdfa" -e "OMP_NUM_THREADS=$2 KMP_AFFINITY=granularity=fine,scatter"
