#!/bin/bash

notes="../notes/*.md"

gcc notes.c md_renderer.c md4c/md4c.c helpers.c -o notes

rm ../local/*.html
rm ../local/tags/*.html
rm ../remote/*.html
rm ../remote/tags/*.html

./notes $notes

echo "Done"

falkon ../local/index.html

