#!/bin/sh
#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./test.mic
