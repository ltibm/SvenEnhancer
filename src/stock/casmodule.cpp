#include <svenenhancer.h>

asIScriptModule* GetModulePtr(CASServerManager* mgr)
{
	if (mgr && mgr->curModule)
	{
		SECASModule* casm = reinterpret_cast<SECASModule*>(mgr->curModule);
		auto* module = casm ? casm->GetModule() : nullptr;
		return module;
	}
	return nullptr;
}
