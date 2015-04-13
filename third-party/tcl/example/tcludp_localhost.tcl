set port 12345
set server "127.0.0.1"

set recv_sock [udp_open $port]
fconfigure $recv_sock -buffering none

set send_sock [udp_open]
fconfigure $send_sock -buffering none -remote [list $server $port]

puts $send_sock "Hello udptcl echo!"
after 1000 [gets $recv_sock line]
puts $line

