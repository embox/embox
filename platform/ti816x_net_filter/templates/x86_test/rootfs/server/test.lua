require("server/show")
local Cgi = require("server/cgi")

print("Start")

local Prm = Cgi.Params()
local List = {}
local Get = {}
ObjectDescribe(List, Prm, "CGI parameters")
local serial = "*****"
print(List)

-- Разбираем пакет по полям
for J, Rec in ipairs(List) do
	print(Rec[1])
	print(Rec[2])
end
