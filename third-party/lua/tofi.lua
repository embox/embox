io.write("Enter new file name:\n")
local filename = io.read()
local ouf = assert(io.open(filename, "w"))
io.write("Type strings and press ENTER. All strings will be put in the file named " .. filename .. "\n")
local finish = ":save"
io.write("To stop fill file please type <" .. finish .. "> and press ENTER.\n")
local str = ""
while str ~= finish do
   str=io.read()
   if str ~= finish then
      ouf:write(str.."\n")
   end
end
ouf:close()

