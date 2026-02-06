local luaDir = GetUserDirectory() .. "/Lua"
print("Lua Loader Initializing.")

local luaFiles = GetFilesInDirectory(luaDir, ".lua")

for k, v in ipairs(luaFiles) do
	print("Executing " .. v)
	local script = ReadFile(v)

	local ok, err = pcall(function(fn)
		loadstring(fn)()
	end, script)

	if not ok then
		print("Error executing " .. v)
		print(err)
	end
end