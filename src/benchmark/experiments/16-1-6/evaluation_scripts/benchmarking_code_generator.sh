#!/bin/sh

threads="1 2 4 8 16 32 60 120 180 240"

#matcher test
filesizes="2G 4G 8G"
for fsize in $filesizes
do
    for threadnum in $threads
    do
        echo "sbatch matcher.sh arti ${threadnum}q $fsize $threadnum"
    done

    for threadnum in $threads
    do
        echo "sbatch matcher.sh test ${threadnum}q $fsize $threadnum"
    done
done


#tokenizer test
scripts="bin_tokenizer.sh hash_tokenizer.sh"
artitokens="1t 7t 15t 23t 31t"
testtokens="1t 18t 36t 54t 72t"

for sc in $scripts
do
    for threadnum in $threads
    do
        for at in $artitokens
        do
            echo "sbatch $sc arti $at 2G $threadnum"
        done
    done
done


for sc in $scripts
do
    for threadnum in $threads
    do
        for tt in $testtokens
        do
            echo "sbatch $sc test $tt 2G $threadnum"
        done
    done
done



