#!/bin/sh
if [ -z $1 ]; then
    echo 'usage: ./toCSV.sh raw_input.txt';
    exit
fi

sed "s/[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*\([0-9]*\)[^0-9]*/\1,\2,\3,\4,\5,\6,\7/" $1 | sed '/[,]\{6\}/d' | sed '/^$/d'  > "$1.tmp"
echo "ns,ms,READ_SIZE,stride,threads,superchunks,full size" > "$1.csv"
cat "$1.tmp" >> $1.csv
rm "$1.tmp"
