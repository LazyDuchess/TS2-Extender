#pragma once

// Name is guessed, whole thing is guessed.

class cUserInput {
public:
	static cUserInput* GetInstance();
	const char* GetString();

	static inline cUserInput** m_GlobalUserInputPtr = nullptr;
private:
	bool IsStringEmbedded();
};