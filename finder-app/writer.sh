#!/bin/sh

WRITEDIR=$1
writestr=$2

len=${#WRITEDIR}
count=1
count2=1
keep=0
sym="/"

while [ $count -lt "$len" ]
do
	str=`expr substr $WRITEDIR $count $count2`
	if [ "$str" = "$sym" ]
	then
		keep=$count		
	fi 
	count=$((count+1))
done	

if [ $# -lt 2 ]
then
	echo "please input two command line arguments"
	echo "first- path to create  second- text string to add"
	exit 1
fi

len2=`expr $count - $keep`
str2=`expr substr $WRITEDIR 1 $keep`


mkdir -p "${str2}"

cd $str2
tmp=`expr $keep + 1`
str3=`expr substr $WRITEDIR $tmp $count`

touch $str3
echo $writestr >> $str3
