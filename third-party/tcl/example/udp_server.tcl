# This example demonstrates usage of TclUDP library.
# Server is running on Embox on port 12345. It executes commands received from client.
#
# See also udp_client.tcl

proc execCmd {sock} {
    global dst_sock
    gets $sock line
    set peer [fconfigure $sock -peer]
    puts "got command from $peer: \"$line\""
    fconfigure $dst_sock -buffering none -remote $peer
    puts -nonewline $dst_sock [exec {*}$line]
    flush $dst_sock
}

set port 12345
set sock [udp_open $port]
puts "1"
fconfigure $sock -buffering none
puts "2"
fileevent $sock readable [list execCmd $sock]
puts "3"

set dst_sock [udp_open]

puts "server started..."
vwait forever

