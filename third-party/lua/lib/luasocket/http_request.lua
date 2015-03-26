
http = require("socket.http")

local Body, Cde, HDR, StartsStr = http.request(arg[1])
print(Body)
print(Cde)

os.exit(Body ~= nil)
