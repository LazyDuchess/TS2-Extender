#pragma once
class cRZString {
public:
	void FromChar(const char* str);
	const char* GetString();
private:
	bool IsEmbedded();
};