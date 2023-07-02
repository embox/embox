local http = require("socket.http")

if not arg or not arg[1] then
	print("lua http_request.lua <url>")
	os.exit(0)
end

local body, code = http.request(arg[1])
print(code)
print(body)

os.exit(code == 200)
