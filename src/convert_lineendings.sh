#!/bin/bash

notes="../notes/*.md"

for note in $notes
do
  awk '{ sub("\r$", ""); print }' $note > $note.tmp
  cp $note.tmp $note
  rm $note.tmp
done


