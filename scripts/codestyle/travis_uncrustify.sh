echo $PWD
if ! $TRAVIS_PULL_REQUEST; then
	bash ./run_uncrustify.sh -v $(git rev-parse HEAD~1) $(git rev-parse HEAD)
else
	bash ./run_uncrustify.sh -v $(git rev-parse HEAD~1) $TRAVIS_PULL_REQUEST_SHA
fi
