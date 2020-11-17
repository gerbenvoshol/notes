#!/bin/bash

DIR=$( dirname $(readlink -f "$0"))
cd $DIR

notes=$( ls -c ../notes/*.md )

gcc -Wall -ggdb3 notes.c md4c/md4c.c md4c/entity.c md4c/md4c-html.c helpers.c -o notes || {
  exit 1
}

rm ../local/*.html
rm ../local/tags/*.html
rm ../remote/*.html
rm ../remote/tags/*.html

./notes $notes

rm notes

if [ -n "$1" ]
then
  falkon ../local/index.html
fi

