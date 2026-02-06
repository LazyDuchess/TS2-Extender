local luaDir = GetUserDirectory() .. "/Lua"
print("Lua Loader Initializing.")

local luaFiles = GetFilesInDirectory(luaDir, ".lua")

for k, v in ipairs(luaFiles) do
	print("Executing " .. v)
	local ok, err = pcall(dofile, v)

	if not ok then
		print("Error executing " .. v)
		print(err)
	end
end