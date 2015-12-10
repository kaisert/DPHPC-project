#!/bin/sh

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ]; then
    echo 'usage: sbatch atest.sh <mdfa/tokens infix for number of queries, e.g. 4q-> arti4q1G.tokens/mdfa > <xmlsize (e.g. 1G-> /mnt/hostfs/bakadi/arti1G.xml)> <num_threads (e.g. 240)>';
    exit
fi

#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./test.mic -a "/mnt/hostfs/bakadi/arti$2.xml /mnt/hostfs/bakadi/arti$1$2.tokens /mnt/hostfs/bakadi/arti$1$2.mdfa" -e "OMP_NUM_THREADS=$3 KMP_AFFINITY=granularity=fine,scatter"
