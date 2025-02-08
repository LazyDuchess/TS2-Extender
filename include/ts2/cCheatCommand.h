#pragma once

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
		virtual void Execute(void* args) = 0;
		virtual const char* Name();
		virtual const char* Description(int helpType);
		virtual bool HandlesAllArguments();
		virtual bool GetConditionalLevelAdjustment();
		virtual void* Destructor(bool freeMemory);
		virtual const char* Description();
		virtual void* CheatSystemParser();
	};
}