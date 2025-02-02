#pragma once

class Core {
public:
	static bool Create();
	static bool Initialize();
private:
	static Core* _instance;
};