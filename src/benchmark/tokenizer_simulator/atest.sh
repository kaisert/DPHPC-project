#!/bin/sh

# use: sbatch atest.sh <filesize in MiB> <number of threads>
#      sbatch atest.sh 2048 60

#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./test.mic -a "$1 80 10" -e "OMP_NUM_THREADS=$2 KMP_AFFINITY=granularity=fine,scatter"


