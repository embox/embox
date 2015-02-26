local filename = arg[1]
io.write("filename: " .. filename .. "\n")
local ouf = assert(io.open(filename, "w"))
ouf:write("1\n")
ouf:write("2\n")
ouf:write("3\n")
ouf:close()

