namespace TS2 {
	class cCheatCommand {
	private:
		void* Unknown;
		int ReferenceCount;
	public:
		virtual void* QueryInterface(int typeId, void** unknown);
		virtual void AddRef();
		virtual int Release();
		virtual void OnRegister(void* parser, void* state);
		virtual void Execute(void* args);
		virtual const char* Name();
		virtual const char* Description();
		virtual bool HandlesAllArguments();
		virtual bool GetConditionalLevelAdjustment();
		virtual void ~cCheatCommand();
	};
}