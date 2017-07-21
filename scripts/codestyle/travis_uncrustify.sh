#!/bin/sh

git fetch origin master:master
git fetch --depth=1000 # unlikely the branch will have more than 1000 commits

revs="$(git rev-parse --short master)...$(git rev-parse --short HEAD)"

echo Checking $revs for code style conformance

$(dirname $0)/run_uncrustify.sh $revs
result=$?

if [ $result != 0 ] && [ x"$TRAVIS_PULL_REQUEST" = xfalse ]; then
	echo "Ignoring code style violations (non-PR job)"
	result=0
fi

exit $result
