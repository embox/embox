set server "10.0.2.16"
set port   12345

set sock [socket $server $port]
puts $sock "hello"
flush $sock

