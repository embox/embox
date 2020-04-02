local socket = require("socket")

port = arg[1] or 1027
udp = assert(socket.udp())
assert(udp:setsockname('*', port))
print("Lua UDP server started on port " .. port .. "...")
while 1 do
	cmd, ip, port = assert(udp:receivefrom())
	print("Execute '" .. cmd .. "' from " .. ip .. ":" .. port)
	os.execute(cmd)
end
