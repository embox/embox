{
	for (i = 1; i < NF; ++i) {
		if ($i == "version") {
			n = split($(i+1), a, ".")
			for (j = 1; j <= n; ++j)
				printf "%02d", a[j]
		}
	}
}
