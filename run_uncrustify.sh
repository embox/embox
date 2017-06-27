#!/bin/bash

if [ $# -eq 0 ]
  then
	files=$(git diff --name-only)
  else 
	files=$(find src -type f -name "*.[ch]")
fi

if [ -s uncrustify_diff.txt ]
then
	rm uncrustify_diff.txt
fi

mkdir -p out

for item in $files ; do

  dn=$(dirname $item)
  mkdir -p out/$dn
  uncrustify -f $item -c uncrustify_cfg.ini > out/$item
  if [ -s out/$item ]
  then
  	git diff --no-index -- $item out/$item >> uncrustify_diff.txt
  fi

done

rm -rf out

if [ -s uncrustify_diff.txt ]
then
	cat uncrustify_diff.txt
	exit 1
fi
#find out -type f -empty -delete
#rsync -ah out/ ./