if $TRAVIS_PULL_REQUEST; then
	./run_uncrustify.sh -v $(git rev-parse HEAD~1) $TRAVIS_PULL_REQUEST_SHA
else
	./run_uncrustify.sh -v $(git rev-parse HEAD~1) $(git rev-parse HEAD)
fi