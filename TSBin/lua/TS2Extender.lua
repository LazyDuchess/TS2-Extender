function PackString(text)
	local packed = {}
	
	local byteCount = string.len(text)
	
	for i = 1, byteCount do
		table.insert(packed, string.byte(text, i))
	end
	
	return packed
end

function UnpackString(data)
	local result = ""
	
	local byteCount = table.getn(data)
	
	for i = 1, byteCount do
		local ch = string.char(data[i])
		result = result .. ch
	end
	
	return result
end

local function TS2ExtenderCheatExecute()
	nUI.ShowTextNotification(1,"TS2 Extender Version: " .. GetTS2ExtenderVersion())
end

local function TS2ExtenderUserInputCheat()
	local userInput = GetUserInput()
	local packed = PackString(userInput)
	local unpacked = UnpackString(packed)
	nUI.ShowTextNotification(1,"User Input: " .. userInput)
	nUI.ShowTextNotification(1,"Packed User Input: " .. table.concat(packed, ", "))
	nUI.ShowTextNotification(1,"Unpacked User Input: " .. unpacked)
end

RegisterCheat("TS2Extender", "Displays TS2Extender info.", TS2ExtenderCheatExecute)
RegisterCheat("UserInput", "Displays user input retrieved by TS2Extender.", TS2ExtenderUserInputCheat)