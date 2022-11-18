#!/usr/bin/bash

for dir in `ls traces/`; do
    i=`ls traces/${dir}/*.tid*.stat*`
    echo $i >> REUSE.txt ;
    time zcat $i | awk '{if ($2==7) print $3}' | sort -n | uniq -c | awk '{tot += $1; count++} END {print "PCs totais: " tot " / PCs unicos: " count}' >> REUSE.txt;
    echo >> REUSE.txt;
done

# for i in `ls traces/calculix/*.tid*.stat*` ; do
    # echo $i >> REUSE.txt ;
    # time zcat $i | awk '{if ($2==7) print and($3, 1023)}' | sort -n | uniq -c >> REUSE.txt;
    # time zcat $i | awk '{if ($2==7 && $1 !="@" && $1!="#") print $3, and($3, 1023)}' | sort -n | uniq > REUSE.txt;
    # time zcat $i | awk '{if ($1=="RET_NEAR" && $1 !="@" && $1!="#") print $1, $2, $3, "-->", $0}' | sort -n | uniq > original_ret.txt;

    # time zcat $i | awk '{if ($2==7) print and($3, 1023)}' | sort -n | uniq -c | awk '{tot += $1; count++} END {print "total de PCs: " tot}' >> REUSE.txt;
# done