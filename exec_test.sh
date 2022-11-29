#!/usr/bin/bash

# for dir in `ls traces/`; do
# 	# echo ${dir}
# 	file=`ls traces/${dir}/*.tid*.stat* | awk -F'.' '{for(i=1;i<=3;i++) printf("%s%s", $i, (i!=3)?".":OFS)}'`
# 	./OrCS/orcs -t $file
# 	# echo
# done

# for dir in `ls traces/`; do
# 	echo ${dir}
# 	for i in {1..100..10}; do
# 		file=`ls traces/${dir}/*.tid*.stat* | awk -F'.' '{for(i=1;i<=3;i++) printf("%s%s", $i, (i!=3)?".":OFS)}'`
# 		sed -i 's/#define GH_LENGTH 1/#define GH_LENGTH '$i'/g' OrCS/processor.hpp
# 		make clean -C OrCS/ > /dev/null 2>&1 ; make -C OrCS/ > /dev/null 2>&1
# 		./OrCS/orcs -t $file
# 		sed -i 's/#define GH_LENGTH '$i'/#define GH_LENGTH 1/g' OrCS/processor.hpp
# 		# echo
# 	done
# 	echo
# done


for dir in `ls traces/`; do
	echo ${dir}
	n=1
	m=65536
	while [ $n -le 65536 ]; do
		file=`ls traces/${dir}/*.tid*.stat* | awk -F'.' '{for(i=1;i<=3;i++) printf("%s%s", $i, (i!=3)?".":OFS)}'`
		
		sed -i 's/#define ADDR_RANGE 1024/#define ADDR_RANGE '$n'/g' OrCS/processor.hpp
		sed -i 's/#define GA_RANGE 4/#define GA_RANGE '$m'/g' OrCS/processor.hpp
		
		make clean -C OrCS/ > /dev/null 2>&1 ; make -C OrCS/ > /dev/null 2>&1
		./OrCS/orcs -t $file
		
		sed -i 's/#define ADDR_RANGE '$n'/#define ADDR_RANGE 1024/g' OrCS/processor.hpp
		sed -i 's/#define GA_RANGE '$m'/#define GA_RANGE 4/g' OrCS/processor.hpp

		n=$((n*2))
		m=$((m/2))
		# echo
	done
	echo
done