
which docker-machine &>/dev/null && eval "$(docker-machine env default)"

if [ ! $(docker ps -q -f 'name=emdocker') ]; then
	echo "WARNING: No running container found!"
fi

dr() {
	local tty_opt=
	[ -t 0 ] && tty_opt="-t"

	docker exec \
		-u user \
		-i \
		$tty_opt \
		$EMDOCKER_EXEC_EXTRA_ARGS \
		emdocker \
		bash -lc "$*"
}
