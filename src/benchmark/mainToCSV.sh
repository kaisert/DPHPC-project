#!/bin/sh
if [ -z $1 ]; then
    echo 'This script converts the raw outputs of our main application to a CSV file for easier evaluation'
    echo 'usage: ./mainToCSV.sh raw_input.txt'
    exit
fi


echo "XML,MDFA,TokenContainer,threads,tokens,matches,loading,chunking,tokenizer,matcher,writeback,total time" > "$1.csv"
cat $1 | tr '\n' '!' | sed 's/!![!]*/\n/g' | tr '!' ',' | sed 's/,,/\n/g' | sed 's/[^:]*: \([^,]*\)[^:]*: \([^,]*\),[^=]*=\([^,]*\),[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)/\1,\2,\3,\4,\5,\6:::/' | sed 's/:::\([0-9]*\)[^:]*:[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)[^:]*: \([0-9]*\)[^:]*: \([0-9]*\).*/\1,\2,\3,\4,\5,\6,\7/' >> "$1.csv"
