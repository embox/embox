#!/bin/bash

if [ $# -gt 3 ]; then
	echo "Too many parameters specified"
	exit 1
fi

VERBOSE=false
HASHES=()

while [[ $# -gt 0 ]]
do
	key="$1"
	case $key in
	    -v|--verbose)
	    VERBOSE=true
	    ;;
	    *)
	    HASHES+=($key)
	    ;;
	esac
	shift
done

if [ ${#HASHES[@]} -eq 2 ];
then 
	diffargs="${HASHES[0]} ${HASHES[1]}"
fi

files=$(git diff --name-only $diffargs)
echo $diffargs
#files=$(find ../../ -type f -name "*.[ch]") #all files - I'll leave it just in case

if [ -s uncrustify_diff.txt ]
then
	rm uncrustify_diff.txt
fi

mkdir -p out

for item in $files ; do
  item="../../"$item #git shows path starting from repo's root and we're at depth of 2.
  if ! ([ ${item: -2} == '.c' ] ||  [ ${item: -2} == '.h' ]); then
  	echo $item" is not a .c or .h file"
  	continue;
  fi
  dn=$(dirname $item)
  mkdir -p out/$dn
  uncrustify -f $item -c uncrustify_cfg.ini > out/$item
  if [ -s out/$item ] #if not empty
  then
  	git diff --no-index -- out/$item $item >> uncrustify_diff.txt
  	#git diff -- $item out/$item $diffargs >> uncrustify_diff.txt
  fi

done

rm -rf out

if [ -s uncrustify_diff.txt ]
then
	echo "The differences that don't comply with the code style are written to uncrustify_diff.txt"
	if $VERBOSE; then
		cat uncrustify_diff.txt
	fi
	exit 1
fi
#find out -type f -empty -delete
#rsync -ah out/ ./