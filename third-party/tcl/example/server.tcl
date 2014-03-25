# This example demonstrates usage of TCP sockets and vwait.
# Server running on Embox accepts connections from foreign hosts on port 12345,
# then waits for any data and exit.
#
# See also client.tcl

after 5000 set state timeout
set didRead 0
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
    puts "got data from $addr: \"$line\""
    set didRead 1
}

# waiting for connection
vwait state

switch $state {
    timeout {
        puts "no connection on port 12345: timeout expired"
	exit 1
    }
    accepted {
       puts "connection from: $connectionAddr"
       puts "waiting for data..."
    }
}

# waiting until something received
vwait didRead

close $server

