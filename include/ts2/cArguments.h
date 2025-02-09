#pragma once

namespace nGZCommandParser {
	class cArguments {
	private:
		int unk1;
		int unk2;
		int unk3;
		virtual void SplitIntoArguments();
		virtual void SwitchNoArguments();
		virtual void SwitchArguments();
		virtual void SwitchArguments2();
	public:
		// Number of arguments passed, excluding the cheat command itself.
		int count;
		// Retrieve an argument. Arguments start from 1, as 0 is the cheat command itself.
		virtual char* operator[](int index);
	};
}