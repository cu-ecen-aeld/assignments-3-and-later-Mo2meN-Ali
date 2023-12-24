#!/bin/sh

set -e
set -u

# Check the number of arguments
if [ $# -lt 2 ]; then
    echo "invalid number of arguments, must be 2!"
    exit 1
fi

assignment=$(cat /etc/finder-app/conf/assignment.txt)

cd $1

fCounter=0
strCounter=0

if [ $assignment != assignment4 ]; then
    for file in $(ls); do
        fCounter=$((fCounter+1))
        if [ -d $file ]; then # subdirectory?
            ((fCounter+=$(cd $file && ls | wc -l)))
            strCounter=$((strCounter+=$(cd $file && grep $2 * | wc -l)))
        else
            strCounter=$((strCounter+=$(grep $2 $file | wc -l)))
        fi
    done
elif [ -e $1/'assignment4-results.txt' ]; then
    fCounter=1
    strCounter=$(grep $2 assignment4-results.txt | wc -l)
fi    

echo "The number of files are $fCounter"
echo "and the number of matching lines are $strCounter"