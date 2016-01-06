#!/bin/sh

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ] || [ -z $4 ]; then
    echo 'used mic binary: bin_tokenizer.mic';
    echo 'usage: sbatch bin_tokenizer.sh <file prefix, e.g. test -> test1G.xml> <mdfa/tokens infix for number of queries, e.g. 4q-> test4q.tokens/mdfa > <xmlsize, e.g. 1G-> /mnt/hostfs/bakadi/test1G.xml> <num_threads (e.g. 240)>';
    exit
fi

#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./bin_tokenizer.mic -a "/mnt/hostfs/bakadi/$1$3.xml /mnt/hostfs/bakadi/$1$2.tokens /mnt/hostfs/bakadi/$1$2.mdfa" -e "OMP_NUM_THREADS=$4 KMP_AFFINITY=granularity=fine,scatter"
