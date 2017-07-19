$1 == "+++" {
	base = outpref "/" $2
	system("dirname " base " | xargs mkdir -p")
	next # to not confuse with addition below
}


# Uncrustify --frag starts assuming indentation is 0
# if first line is empty. So had to skip empty lines
# chunk start.
# This set in chunk matcher
skip_empty_lines_at_chunk_start {
	if (NF) {
		skip_empty_lines_at_chunk_start = 0
	} else {
		next
	}
}

$1 == "@@" {
	chunk = base $3

	skip_empty_lines_at_chunk_start = 1
}

substr($0, 0, 1) == "+" || substr($0, 0, 1) == " " {
	print substr($0, 2) >> chunk
}

