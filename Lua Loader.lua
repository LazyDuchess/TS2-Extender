local exeDir = GetExecutableDirectory() .. "/lua"
print("Lua Loader Initializing.")

local function listFiles(dir)
	local files = {}

	os.execute('dir /b "' .. dir .. '" > temp.txt')

	local f = io.open("temp.txt", "r")
	if f ~= nil then
		for line in f:lines() do
			table.insert(files, dir .. "\\" .. line)
		end
		f:close()
		os.remove("temp.txt")
	end

	return files
end

local scripts = listFiles(exeDir)

for _, path in ipairs(scripts) do
	print("Executing " .. path)
	local ok, err = pcall(dofile, path)

	if not ok then
		print("Error executing " .. path)
		print(err)
	end
end
