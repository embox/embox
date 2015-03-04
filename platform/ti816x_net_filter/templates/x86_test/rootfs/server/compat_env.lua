--[[
  compat_env - see README for details.
  (c) 2012 David Manura.  Licensed under Lua 5.1/5.2 terms (MIT license).
--]]

local M = {_TYPE='module', _NAME='compat_env', _VERSION='0.2.2.20120406'}

local function check_chunk_type(s, mode)
  local nmode = mode or 'bt'
  local is_binary = s and #s > 0 and s:byte(1) == 27
  if is_binary and not nmode:match'b' then
    return nil, ("attempt to load a binary chunk (mode is '%s')"):format(mode)
  elseif not is_binary and not nmode:match't' then
    return nil, ("attempt to load a text chunk (mode is '%s')"):format(mode)
  end
  return true
end

local IS_52_LOAD = pcall(load, '')
if IS_52_LOAD then
  M.load     = _G.load
  M.loadfile = _G.loadfile
else
  -- 5.2 style `load` implemented in 5.1
  function M.load(ld, source, mode, env)
    local f
    if type(ld) == 'string' then
      local s = ld
      local ok, err = check_chunk_type(s, mode)
      if not ok then return ok, err end
      local err; f, err = loadstring(s, source)
      if not f then return f, err end
    elseif type(ld) == 'function' then
      local ld2 = ld
      if (mode or 'bt') ~= 'bt' then
        local first = ld()
        local ok, err = check_chunk_type(first, mode)
        if not ok then return ok, err end
        ld2 = function()
          if first then
            local chunk=first; first=nil; return chunk
          else return ld() end
        end
      end
      local err; f, err = load(ld2, source); if not f then return f, err end
    else
      error(("bad argument #1 to 'load' (function expected, got %s)")
            :format(type(ld)), 2)
    end
    if env then setfenv(f, env) end
    return f
  end

  -- 5.2 style `loadfile` implemented in 5.1
  function M.loadfile(filename, mode, env)
    if (mode or 'bt') ~= 'bt' then
      local ioerr
      local fh, err = io.open(filename, 'rb'); if not fh then return fh,err end
      local function ld()
        local chunk; chunk,ioerr = fh:read(4096); return chunk
      end
      local f, err = M.load(ld, filename and '@'..filename, mode, env)
      fh:close()
      if not f then return f, err end
      if ioerr then return nil, ioerr end
      return f
    else
      local f, err = loadfile(filename); if not f then return f, err end
      if env then setfenv(f, env) end
      return f
    end
  end
end

if _G.setfenv then -- Lua 5.1
  M.setfenv = _G.setfenv
  M.getfenv = _G.getfenv
else -- >= Lua 5.2
  -- helper function for `getfenv`/`setfenv`
  local function envlookup(f)
    local name, val
    local up = 0
    local unknown
    repeat
      up=up+1; name, val = debug.getupvalue(f, up)
      if name == '' then unknown = true end
    until name == '_ENV' or name == nil
    if name ~= '_ENV' then
      up = nil
      if unknown then
        error("upvalues not readable in Lua 5.2 when debug info missing", 3)
      end
    end
    return (name == '_ENV') and up, val, unknown
  end

  -- helper function for `getfenv`/`setfenv`
  local function envhelper(f, name)
    if type(f) == 'number' then
      if f < 0 then
        error(("bad argument #1 to '%s' (level must be non-negative)")
              :format(name), 3)
      elseif f < 1 then
        error("thread environments unsupported in Lua 5.2", 3) --[*]
      end
      f = debug.getinfo(f+2, 'f').func
    elseif type(f) ~= 'function' then
      error(("bad argument #1 to '%s' (number expected, got %s)")
            :format(type(name, f)), 2)
    end
    return f
  end
  -- [*] might simulate with table keyed by coroutine.running()

  -- 5.1 style `setfenv` implemented in 5.2
  function M.setfenv(f, t)
    local f = envhelper(f, 'setfenv')
    local up, val, unknown = envlookup(f)
    if up then
      debug.upvaluejoin(f, up, function() return up end, 1) --unique upval[*]
      debug.setupvalue(f, up, t)
    else
      local what = debug.getinfo(f, 'S').what
      if what ~= 'Lua' and what ~= 'main' then -- not Lua func
        error("'setfenv' cannot change environment of given object", 2)
      end -- else ignore no _ENV upvalue (warning: incompatible with 5.1)
    end
    return f  -- invariant: original f ~= 0
  end
  -- [*] http://lua-users.org/lists/lua-l/2010-06/msg00313.html

  -- 5.1 style `getfenv` implemented in 5.2
  function M.getfenv(f)
    if f == 0 or f == nil then return _G end -- simulated behavior
    local f = envhelper(f, 'setfenv')
    local up, val = envlookup(f)
    if not up then return _G end -- simulated behavior [**]
    return val
  end
  -- [**] possible reasons: no _ENV upvalue, C function
end


return M
