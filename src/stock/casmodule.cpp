#include <svenenhancer.h>

std::stack<CASModule*> g_ModuleStack;

void CASCurrentModulePush(CASModule* pNewModule) {
	auto manager = ASEXT_GetServerManager();
	if (manager == nullptr) return;
	g_ModuleStack.push(manager->curModule);
	manager->curModule = pNewModule;
}

void CASCurrentModulePop() {
	auto manager = ASEXT_GetServerManager();
	if (manager == nullptr || g_ModuleStack.empty()) return;
	CASModule* pOldModule = g_ModuleStack.top();
	g_ModuleStack.pop();
	manager->curModule = pOldModule;
}

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
