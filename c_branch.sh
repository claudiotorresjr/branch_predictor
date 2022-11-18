#!/usr/bin/bash

for i in `ls calculix*.tid*.stat*` ; do
    echo $i >> REUSE.txt ;
    time zcat $i | awk '{if ($2==7) print and($3, 1023)}' | sort -n | uniq -c | awk '{tot += $1; count++} END {print "total de PCs: " tot}' >> REUSE.txt;
done