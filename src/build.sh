#!/bin/bash

DIR=$( dirname $(readlink -f "$0"))
cd $DIR

notes=$( ls -c ../notes/*.md )

gcc notes.c md_renderer.c md4c/md4c.c helpers.c -o notes || {
  exit 1
}

rm ../local/*.html
rm ../local/tags/*.html
rm ../remote/*.html
rm ../remote/tags/*.html

./notes $notes

if [ -n "$1" ]
then
  falkon ../local/index.html
fi

