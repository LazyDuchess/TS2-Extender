local function TS2ExtenderCheatExecute()
	nUI.ShowTextNotification(1,"TS2 Extender Version: " .. GetTS2ExtenderVersion())
end

RegisterCheat("TS2Extender", "Displays TS2Extender info.", TS2ExtenderCheatExecute)