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
        -h|--help)
        echo "If SHA is provided to the script, \
            uncrustify checks code style of files that are different in SHA and HEAD. \
            Otherwise uncommited diff is checked. \
            Option -v shows uncrustify suggestions for code style, when it's not set, \
            only file names with differences are shown."
	    *)
	    HASHES+=($key)
	    ;;
	esac
	shift
done

if [ ${#HASHES[@]} -eq 1 ];
then 
	diffargs="${HASHES[0]}"
fi

files=$(git diff --name-only $diffargs)
#echo $diffargs
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
  	git diff --no-index -- out/$item $item > uncrustify_diff_cur.txt
    if ! [ -s uncrustify_diff_cur.txt ]; then
        $item >> uncrustify_diff_filenames.txt
        uncrustify_diff_cur >> uncrustify_diff.txt
    fi
   
  	#git diff -- $item out/$item $diffargs >> uncrustify_diff.txt
  fi

done

rm -rf out

if [ -s uncrustify_diff.txt ]
then
	echo "Some files don't comply with code style:"
	if $VERBOSE; then
		cat uncrustify_diff.txt
    else
        cat uncrustify_diff_filenames.txt
	fi
	exit 1
fi

rm uncrustify_diff*
#find out -type f -empty -delete
#rsync -ah out/ ./
