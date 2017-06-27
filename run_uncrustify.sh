#! /bin/sh

files=$(git diff --name-only)
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

#find out -type f -empty -delete
#rsync -ah out/ ./
rm -rf out