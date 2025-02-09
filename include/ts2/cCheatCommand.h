#pragma once
#include "ts2/cArguments.h"

namespace TS2 {
	class cCheatCommand {
	private:
		void* Parser = nullptr;
		int ReferenceCount = 0;
	public:
		virtual void* QueryInterface(int typeId, void** unknown);
		virtual void AddRef();
		virtual int Release();
		virtual void OnRegister(void* parser, void* unk);
		virtual void Execute(nGZCommandParser::cArguments* args) = 0;
		virtual const char* Name();
		virtual const char* DescriptionForHelp(int helpType);
		virtual int HandlesAllArguments();
		virtual int GetConditionalLevelAdjustment();
		virtual void* Destructor(bool freeMemory);
		virtual const char* Description();
		virtual void* CheatSystemParser();
	};
}