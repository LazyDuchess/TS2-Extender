TS2Extender = true

Delegates = {}

Delegates.OnBuildPieMenu = 0

-- Returns an UTF-8 string as a table, with each character as an element in the table. Can be useful for string manipulation or length checks.
-- spec from https://en.wikipedia.org/wiki/UTF-8
function UTF8StringToTable(str)
	local chars = {}
	local i = 1
	local length = string.len(str)

	while i <= length do
		local b = string.byte(str, i)
		local charLength
		
		-- 0xxxxxxx - 1 byte character
		if BitwiseAnd(b,128) == 0 then
			charLength = 1
		-- 110xxxxx - 2 byte character
		elseif BitwiseAnd(b,224) == 192 then
			charLength = 2
		-- 1110xxxx - 3 byte character
		elseif BitwiseAnd(b,240) == 224 then
			charLength = 3
		-- 11110xxx - 4 byte character
		elseif BitwiseAnd(b,248) == 240 then
			charLength = 4
		else
		-- invalid
			charLength = 1
		end
		table.insert(chars, string.sub(str, i, i + charLength - 1))
		i = i + charLength
	end
	
	return chars
end

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

DataWriter = {}
DataWriter.__index = DataWriter

function DataWriter.new()
	local dw = {
		index = 1,
		data = {}
	}
	setmetatable(dw, DataWriter)
	return dw
end

function DataWriter:Write(value)
	self.data[self.index] = value
	self.index = self.index + 1
end

function DataWriter:WriteString(str)
	local packedString = PackString(str)
	local packLength = table.getn(packedString)
	self:Write(packLength)
	for i = 1, packLength do
		self:Write(packedString[i])
	end
end

function DataWriter:ApplyToToken(token)
	token:clearProperties()
	token:setProperties(self.data)
end

DataReader = {}
DataReader.__index = DataReader

function DataReader.new()
	local dr = {
		index = 1,
		data = {}
	}
	setmetatable(dr, DataReader)
	return dr
end

function DataReader.FromToken(token)
	local dr = DataReader.new()
	dr.data = token:getPropertiesCopy()
	return dr
end

function DataReader:Read()
	local val = self.data[self.index]
	self.index = self.index + 1
	return val
end

function DataReader:ReadString()
	local strlen = self:Read()
	local packedString = {}
	for i = 1, strlen do
		table.insert(packedString, self:Read())
	end
	return UnpackString(packedString)
end

local function TS2ExtenderCheatExecute()
	nUI.ShowTextNotification(1,"TS2 Extender Version: " .. GetTS2ExtenderVersion())
end

RegisterCheat("TS2Extender", "Displays TS2Extender info.", TS2ExtenderCheatExecute)