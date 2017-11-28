#!/bin/bash

make

option=$1

if [[ $1 -le 2 ]]; then
	file=multiproc_
	let "j=$1"
elif [[ $1 -le 4 ]]; then
	file=multithread_
	let "j=$1-2"
fi

echo running.................................................. $file$j

let "loops=$2"
let "rep=$3"

shift 3

for i in `seq 1 $loops`
do
./$file$j $rep $@
#echo $@
done

