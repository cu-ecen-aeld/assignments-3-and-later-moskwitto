#!/bin/sh
# Author: Moskwitto

filesdir=$1;
searchstr=$2;
matches=0;
numfiles=0;

if [ !  $1  ]
    then
       echo "Please sepecify a search directory";
        exit 1;
elif [ ! $2 ]
    then
	echo "Please specify a search string";
	exit 1;
fi

if [ -d "$filesdir" ]
    then
	for file in $(find "$filesdir" -type f);do
	  numfiles=$((numfiles+1));
          file_matches=$(grep -c "$searchstr" "$file" 2>/dev/null);
	  matches=$((matches+file_matches));
	done
	printf "The number of files are $numfiles and the number of matching lines are $matches\n";
	exit 0;
     else
	printf "Search  directory does not exist\n"
        exit 1;
fi
