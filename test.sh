#!/bin/bash
arraysize=( 1200 2400 4800 )
npr=( 2 4 6 8 10 )

for a in ${arraysize[@]}; do
	for n in ${npr[@]}; do
		echo "arraysize:" $a "#processor:" $n
		qsub -pe mpich $n -v size=$a mysge.sh
	done
done
