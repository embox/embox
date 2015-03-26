local CE = require "server/compat_env"
getfenv=CE.getfenv
setfenv=CE.setfenv
print(getfenv,setfenv)
-- This function conditions a key or value for display
local function LclRenderStr(Obj, TruncLen)
	local TpStr = type(Obj)
	if TpStr == "string" then
		Obj = string.gsub(Obj, "[^%w%p ]", function(Ch)	return "\\" .. string.format("%03d", string.byte(Ch)) end )
		if TruncLen and TruncLen > 0 and string.len(Obj) > TruncLen + 3 then
-- This could misleadingly truncate numeric escape value
			Obj = string.sub(Obj, 1, TruncLen) .. "..."
		end
		Obj = '"' .. Obj .. '"'
	elseif TpStr == "boolean" then
		Obj = "boolean: " .. tostring(Obj)
	else
		Obj = tostring(Obj)
	end
	return Obj
end
-- This function replaces [“x”][“y”] stubble with x.y. Keys are assumed to be
-- identifier-compatible.
local function LclShave(Str)
	local Count
	Str, Count = string.gsub(Str, '^%[%"(.+)%"%]$', '%1')
	if Count == 1 then
		Str = string.gsub(Str, '%"%]%[%"', '.')
	end
	return Str
end

local function LclRender(Tbl, Val, KeyStr, TruncLen, Lvl, Visited, KeyPathStr)
	local VtpStr, ValStr
	VtpStr = type(Val)
	if Visited[Val] then
		ValStr = "same as " .. Visited[Val]
	else
		ValStr = LclRenderStr(Val, TruncLen)
		if VtpStr == "function" then -- Display function’s environment
			local Env = getfenv(Val)
			Env = Visited[Env] or Env
			ValStr = string.gsub(ValStr, "(function:%s*.*)$", "%1 (env " .. string.gsub(tostring(Env), "table: ", "") .. ")")
		elseif VtpStr == "table" then
			ValStr = ValStr .. string.format(" (n = %d)", #Val)
		end
	end

	KeyPathStr = KeyPathStr .. "[" .. KeyStr .. "]"
	table.insert(Tbl, { Lvl, string.format('[%s] %s', KeyStr, ValStr) })
	if VtpStr == "table" and not Visited[Val] then
		Visited[Val] = LclShave(KeyPathStr)
		local SrtTbl = {}
		for K, V in pairs(Val) do
			table.insert(SrtTbl, { LclRenderStr(K, TruncLen), V, K, type(K) })
		end
		local function LclCmp(A, B)
			local Cmp
			local Ta, Tb = A[4], B[4]
			if Ta == "number" then
				if Tb == "number" then
					Cmp = A[3] < B[3]
				else
					Cmp = true -- Numbers appear first
				end
			else -- A is not a number
				if Tb == "number" then
					Cmp = false -- Numbers appear first
				else
					Cmp = A[1] < B[1]
				end
			end
			return Cmp
		end
		table.sort(SrtTbl, LclCmp)
		for J, Rec in ipairs(SrtTbl) do
			LclRender(Tbl, Rec[2], Rec[1], TruncLen, Lvl + 1, Visited, KeyPathStr)
		end
	end
end
-- This function appends a series of records of the form { level,
-- description_string } to the indexed table specified by Tbl. When this
-- function returns, Tbl can be used to inspect the Lua object specified by
-- Val. Key specifies the name of the object. TruncLen specifies the maximum
-- length of each description string; if this value is zero, no truncation will
-- take place. Keys are sorted natively (that is, numbers are sorted
-- numerically and everything else lexically). String values are displayed with
-- quotes, numbers are unadorned, and all other values have an identifying
-- prefix such as “boolean”. Consequently, all keys are displayed within their
-- type partition. This function returns nothing; its only effect is to augment
-- Tbl.
function ObjectDescribe(Tbl, Val, Key, TruncLen)
	LclRender(Tbl, Val, LclRenderStr(Key, TruncLen), TruncLen or 0, 1, {}, "")
end
-- This function prints a hierarchical summary of the object specified by Val
-- to standard out. See ObjectDescribe for more details.
function ObjectShow(Val, Key, TruncLen)
	local Tbl = {}
	ObjectDescribe(Tbl, Val, Key, TruncLen)
	for J, Rec in ipairs(Tbl) do
		io.write(string.rep(" ", Rec[1] - 1), Rec[2], "\n")
	end
end
