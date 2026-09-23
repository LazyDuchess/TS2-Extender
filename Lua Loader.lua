local luaDir = GetUserDirectory() .. "/Lua"
print("Lua Loader Initializing.")

local luaFiles = GetFilesInDirectory(luaDir, ".lua")

local function IsPriorityScript(path)
	if string.len(path) < 16 then
		return false
	end
	path = string.lower(path)
	local sub = string.sub(path, -16)
	return sub == "/ts2extender.lua" or sub == "\\ts2extender.lua"
end

local function RunScriptFile(path)
	print("Executing " .. path)
	local script = ReadFile(path)

	local ok, err = pcall(function(fn)
		loadstring(fn)()
	end, script)

	if not ok then
		print("Error executing " .. path)
		print(err)
	end
end

for k, v in ipairs(luaFiles) do
	if IsPriorityScript(v) then
		RunScriptFile(v)
	end
end

for k, v in ipairs(luaFiles) do
	if not IsPriorityScript(v) then
		RunScriptFile(v)
	end
end