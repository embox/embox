
BEGIN { print "\npackage genconfig\n\nconfiguration conf {" }

{
  level = 0
  if (NF == 1) {
	  level = 0
  } else if (NF == 3) {
	  level = "2"
  } else if (NF == 4) {
	  level = $3
  }
  if (level)
	print "\t@Runlevel(" level ")";
  if (NF > 0)
	print "\tinclude " $1 "\n"
}

END { print "}" }
