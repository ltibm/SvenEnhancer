#include <svenenhancer.h>

asIScriptModule* GetActiveModule()
{
	CASServerManager* manager = ASEXT_GetServerManager();
	return GetModulePtr(manager);
}

asIScriptModule* GetModulePtr(CASServerManager* mgr)
{
	if (mgr && mgr->curModule)
	{
		SECASModule* casm = reinterpret_cast<SECASModule*>(mgr->curModule);
		auto* module = casm ? casm->GetModule() : nullptr;
		return module;
	}
	else {
		asIScriptContext* ctx = (asIScriptContext*)ASEXT_GetCurrentContext();
		if (ctx)
		{
			int ct = ctx->GetCallstackSize();
			if (ct >= 0)
			{
				auto fn = ctx->GetFunction(ct - 1);
				return fn->GetModule();
			}
		}
	}
	return nullptr;
}
