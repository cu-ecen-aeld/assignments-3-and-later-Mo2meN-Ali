#!/bin/sh
# Tester script for assignment 3 part 2
# Author: Siddhant Jajoo

set -e
set -u

# create $WRITEDIR if not assignment1 or assignment4
assignment=$(cat /etc/finder-app/conf/assignment.txt)
username=$(cat /etc/finder-app/conf/username.txt)
WRITESTR=AELD_IS_FUN

if [ $assignment != 'assignment4' ]; then
	NUMFILES=10
	WRITEDIR=/tmp/aeld-data
else
	NUMFILES=1
	WRITEDIR=/tmp
fi

cd `dirname $0`

if [ $# -lt 3 ]
then
	echo "Using default value ${WRITESTR} for string to write"
	if [ $# -lt 1 ]
	then
		echo "Using default value ${NUMFILES} for number of files to write"
	else
		NUMFILES=$1
	fi	
else
	NUMFILES=$1
	WRITESTR=$2
	WRITEDIR=/tmp/aeld-data/$3
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

if [ $assignment != 'assignment4' ]; then
	rm -rf "${WRITEDIR}"
else
	rm -rf "${WRITEDIR}/assignment4-results.txt"
fi

if [ $assignment != 'assignment1' ] && [ $assignment != 'assignment4' ]
then
	mkdir -p "$WRITEDIR"

	#The WRITEDIR is in quotes because if the directory path consists of spaces, then variable substitution will consider it as multiple argument.
	#The quotes signify that the entire string in WRITEDIR is a single string.
	#This issue can also be resolved by using double square brackets i.e [[ ]] instead of using quotes.
	if [ -d "$WRITEDIR" ]
	then
		echo "$WRITEDIR created"
	else
		exit 1
	fi
fi

if [ $assignment != 'assignment4' ]; then
	echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"
	for i in $( seq 1 $NUMFILES)
	do
		./writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
	done
else
	./writer "$WRITEDIR/assignment4-results.txt" "$WRITESTR"
fi

OUTPUTSTRING=$(./finder.sh "$WRITEDIR" "$WRITESTR")

set +e
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
	echo "success"
	exit 0
else
	echo "failed: expected  ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
	exit 1
fi
