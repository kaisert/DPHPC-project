#!/bin/env sh

if [ -z $1 ] || [ -z $2 ]; then
    echo 'usage: sh run_test.sh <path-to-xpath.xpath> <path-to-xml-file.xml>';
    exit
fi

python2 ../utils/qtree2nfa/generate_multi_dfa_lut.py $1

mdfa_file=`echo $1|sed 's/.xpath/.mdfa/'`
tokens_file=`echo $1|sed 's/.xpath/.tokens/'`

echo $1 $mdfa_file $tokens_file
./bin/DPHPC15 $2 $tokens_file $mdfa_file ~/tmp/out_local.log

