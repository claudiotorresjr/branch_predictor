#!/usr/bin/bash

for dir in `ls traces/`; do
	file=`ls traces/${dir}/*.tid*.stat* | awk -F'.' '{for(i=1;i<=3;i++) printf("%s%s", $i, (i!=3)?".":OFS)}'`
	./OrCS/orcs -t $file
	echo
done
