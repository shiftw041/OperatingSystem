#!/bin/bash
cd $1
for files in `ls -l | grep ^[^d] | awk '{print $9}'`
do
	final=${files##*.}
	filename=${files%.*}
	filename=${filename/-*-*-*-*-*}
	cur_date="-`date +%Y-%m-%d-%H-%M`"
	file=$filename$cur_date
	mv $files ${file}.$final
done