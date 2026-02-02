#pragma once

// Name is guessed, whole thing is guessed.

class cUserInput {
public:
	static cUserInput* GetInstance();
	const char* GetString();
	void SetString(const char* str);

	static inline cUserInput** m_GlobalUserInputPtr = nullptr;
private:
	bool IsStringEmbedded();
};