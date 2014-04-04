set server "192.168.0.210"
set port   12345
set prompt ">>"

proc print_data_from_sock {sock} {
	# waiting while some data will be avaliable
	gets $sock buf

	fconfigure $sock -blocking 0
	while 1 {
		puts $buf
		if {[gets $sock buf] < 0} break
	}
	fconfigure $sock -blocking 1
}

puts "connecting to $server"
set sock [socket $server $port]
puts "connected to server"
fconfigure $sock -buffering none
fconfigure stdout -buffering none

puts "enter commands:"
puts -nonewline $prompt
flush stdout

while {[gets stdin cmd] >= 0} {
	puts $sock $cmd
	print_data_from_sock $sock
	puts -nonewline $prompt
}

