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
		echo "run_uncrustify.sh [options] [<SHA1> <SHA2>]"
		echo ""
		echo "If SHA is provided to the script, uncrustify checks code style of files that are different in SHA and HEAD. "
		echo "Otherwise uncommited diff is checked."
		echo ""
		echo "Options:"
		echo "-v|--verbose : shows uncrustify suggestions for code style. When not set, only file names with differences are shown."
		echo "-h|--help		: shows this message"
		exit 1
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

files=$(git diff --name-only $diffargs);

folders_to_format=$(cat folders_to_format.txt)
echo $folders_to_format;

if [ -s uncrustify_diff.txt ]
then
	rm uncrustify_diff.txt
fi

mkdir -p out

for item in $files ; do
	if_process_item=false
	for folder in $folders_to_format ; do
	if [[ $item == $folder* ]] ; then			
		if_process_item=true;
		break;
	fi
	done
	if [ "$if_process_item" = false ] ; then
		continue
	fi
	item="../../"$item #git shows path starting from repo's root and we're at depth of 2.
	if ! ([ ${item: -2} == '.c' ] ||	[ ${item: -2} == '.h' ]); then
		echo $item" is not a .c or .h file"
		continue;
	fi
	dn=$(dirname $item)
	mkdir -p out/$dn
	uncrustify -f $item -c uncrustify_cfg.ini > out/$item
	if [ -s out/$item ]
	then
		git diff --no-index -- out/$item $item > uncrustify_diff_cur.txt
		if [ -s uncrustify_diff_cur.txt ]; then
				echo $item >> uncrustify_diff_filenames.txt
				cat uncrustify_diff_cur.txt >> uncrustify_diff.txt
		fi
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
	rm uncrustify_diff*
	exit 1
fi

rm uncrustify_diff* &> /dev/null

exit 0 #neccessary if 'rm' fails
