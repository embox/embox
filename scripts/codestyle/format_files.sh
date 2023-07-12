#! /bin/sh
# based on script from https://github.com/uncrustify/uncrustify/etc/dofiles.sh
# usage: the only argument is a file with list of files to process

if [ -z "$1" ]; then
	echo "specify the file that contains a list of files"
	exit
fi

mkdir -p out

while read -r item;
do
	dn=$(dirname "$item")
	mkdir -p "out/$dn"
	uncrustify -f "$item" -c uncrustify_cfg.ini > "out/$item"
done < "$1"

find out -type f -empty -delete
rsync -ah out/ ./
rm -rf out
