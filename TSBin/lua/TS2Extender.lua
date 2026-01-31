function PackString(text)
	local packed = {}
	
	local byteCount = string.len(text)
	
	local i = 1
	while i <= byteCount do
		local byte1 = BitShiftLeft(string.byte(text, i), 8)
		local byte2 = string.byte(text, i+1)
		
		if byte2 == nil then
			byte2 = 0
		end
		
		local value = BitwiseOr(byte1, byte2)

		table.insert(packed, value)
		
		i = i + 2
	end
	
	return packed
end

function UnpackString(data)
	local result = ""
	
	local byteCount = table.getn(data)
	
	for i = 1, byteCount do
		local short = data[i]
		local byte1 = BitwiseAnd(BitShiftRight(short, 8), 255)
		local byte2 = BitwiseAnd(short, 255)

		local ch = string.char(byte1)
		result = result .. ch
		
		if byte2 ~= 0 then
			local ch2 = string.char(byte2)
			result = result .. ch2
		end
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