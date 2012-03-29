
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
  str = "\t"
  if (level)
	str = "\t@Runlevel(" level ") ";
  if (NF > 0)
	print str "include " $1 #"\n"
}

END { print "}" }
