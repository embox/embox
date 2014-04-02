# This example demonstrates usage of TCP sockets and vwait.
# Server running on Embox accepts connections from foreign hosts on port 12345,
# then executes commands received from client.
#
# See also client.tcl

set server [socket -server accept 12345]

proc accept {chan addr port} {
    global state connectionAddr
    set state accepted
    set connectionAddr $addr
    fileevent $chan readable "readLine $addr $chan"
}

proc readLine {addr chan} {
    global didRead
    gets $chan line
    puts "got command from $addr: \"$line\""
    puts $chan [exec {*}$line]
    flush $chan
}

# waiting for connection
puts "waiting for connection..."
vwait state

switch $state {
    accepted {
       puts "connection from: $connectionAddr"
       puts "waiting for data..."
    }
    default {
       puts "unrecognized socket's state"
    }
}
vwait forever

close $server

