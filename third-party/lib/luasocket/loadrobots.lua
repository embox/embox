local socket = require("socket")
client = socket.connect("google.com", 80)
client:send("GET /robots.txt HTTP/1.0\r\n\r\n")
while true do
	s, status, partial = client:receive(1024)
	print(s or partial)
	if status == "closed" then
		break
	end
end
client:close()
