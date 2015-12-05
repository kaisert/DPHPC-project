#!/bin/sh
# Batch your job with:
#   sbatch test.sh Hello_Param 240

#SBATCH --gres=mic:1 -n 1 -N 1
export SINK_LD_LIBRARY_PATH=/opt/intel/composer_xe_2015/lib/mic/
srun --gres=mic:1 micnativeloadex ./test.mic -a "$1" -e "OMP_NUM_THREADS=$2 KMP_AFFINITY=granularity=fine,scatter"
#-a "parameters passed to your executable"
#-e "OMP_ENVIRONMENT_VARIABLES=documented_value"
#
# For more: micnativeloadex --help 
