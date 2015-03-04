local Cgi = {}
-- Return a urlencoded copy of Str. For example,
--
--	"Here & there + 97% of other places"
-- is encoded as
--
--	"Here%20%26%20there%20%2B%2097%25%20of%20other%20places"
function Cgi.Encode(Str)
return string.gsub(Str, '%W', function(Str)
return string.format('%%%02X', string.byte(Str)) end )
end
-- Returns a urldecoded copy of Str. This function reverses the effects of
-- Cgi.Encode.
function Cgi.Decode(Str)
Str = string.gsub(Str, '%+', ' ')
Str = string.gsub(Str, '%%(%x%x)', function(Str)
return string.char(tonumber(Str, 16)) end )
return Str
end
-- Returns an escaped copy of Str in which the characters &<>” are escaped for
-- display in HTML documents.
function Cgi.Escape(Str)
Str = string.gsub(Str or "", "%&", "&amp;")
Str = string.gsub(Str, '%"', "&quot;")
Str = string.gsub(Str, "%<", "&lt;")
return string.gsub(Str, "%>", "&gt;")
end

-- This function returns an associative array with the parsed contents of the
-- urlencoded string Str. Multiple values with the same name are placed into an
-- indexed table with the name.
local function LclParse(Str)
local Decode, Tbl = Cgi.Decode, {}
for KeyStr, ValStr in string.gmatch(Str .. '&', '(.-)%=(.-)%&') do
local Key = Decode(KeyStr)
local Val = Decode(ValStr)
local Sub = Tbl[Key]
local SubCat = type(Sub)
-- If there are multiple values with the same name, place them in an
-- indexed table.
if SubCat == "string" then -- replace string with table
Tbl[Key] = { Sub, Val }
elseif SubCat == "table" then -- insert into existing table
table.insert(Sub, Val)
else -- add as string field
Tbl[Key] = Val
end
end
return Tbl
end
-- Returns an associative array with both the GET and POST contents which
-- accompany an HTTP request. Multi-part form data, usually used with uploaded
-- files, is not currently supported.
function Cgi.Params()
local PostLen, GetStr, PostStr, KeyList, Obj
KeyList = {
'PATH_INFO',
'PATH_TRANSLATED',
'REMOTE_HOST',
'REMOTE_ADDR',
'GATEWAY_INTERFACE',
'SCRIPT_NAME',
'REQUEST_METHOD',
'HTTP_ACCEPT',
'HTTP_ACCEPT_CHARSET',
'HTTP_ACCEPT_ENCODING',
'HTTP_ACCEPT_LANGUAGE',
'HTTP_FROM',
'HTTP_HOST',
'HTTP_REFERER',
'HTTP_USER_AGENT',
'QUERY_STRING',
'SERVER_SOFTWARE',
'SERVER_NAME',
'SERVER_PROTOCOL',
'SERVER_PORT',
'CONTENT_TYPE',
'CONTENT_LENGTH',
'AUTH_TYPE' }

Obj = {}
for J, KeyStr in ipairs(KeyList) do
Obj[KeyStr] = os.getenv(KeyStr)
end
if not Obj.SERVER_SOFTWARE then -- Command line invocation
Obj.QUERY_STRING = arg[1]
PostStr = arg[2]
elseif "application/x-www-form-urlencoded" == Obj.CONTENT_TYPE then
-- Web server invocation with posted urlencoded content
PostLen = tonumber(Obj.CONTENT_LENGTH)
if PostLen and PostLen > 0 then
PostStr = io.read(PostLen)
end
end
PostStr = PostStr or ""
Obj.Post = LclParse(PostStr)
Obj.POST_STRING = PostStr
GetStr = Obj.QUERY_STRING or ""
Obj.Get = LclParse(GetStr)
return Obj
end
return Cgi
