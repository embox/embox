#----------------------------------------------------------------------
# Description: night build script
# Author: Nikolay Korotky
# Created at: Tue Nov 30 21:15:15 MSK 2010
#
# Copyright (c) 2010 Nikolay Korotky,  All rights reserved.
#
#!/bin/sh

OUT=summaru.log

pushd embox > /dev/null

echo `date` >> $OUT

for project in `ls templates`; do
    for profile in `ls templates/$project`; do
	make PROJECT=$project PROFILE=$profile config
	echo "project = $project, profile = $profile" >> $OUT
	make &> make.log
	echo "    $(cat make.log | grep 'Build complete')" >> $OUT
	echo "    warnings: $(cat make.log | grep warning: | wc -l)" >> $OUT
	make dc
    done
done

echo `date` >> $OUT

popd > /dev/null

