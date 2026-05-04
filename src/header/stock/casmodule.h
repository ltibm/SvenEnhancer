#pragma once
#include <stack>
class asIScriptModule;
class CASServerManager;
class SECASModule  {
public:
	asIScriptModule* GetModule() const { return m_pModule; }
private:
	int unk1;//0
	int unk2;//4
	asIScriptModule* m_pModule;

	//const CASModuleDescriptor* m_pDescriptor;

	//CASScheduler* m_pScheduler;

	//ASModuleUserData* m_pUserData = nullptr;
};

asIScriptModule* GetModulePtr(CASServerManager*);
asIScriptModule* GetActiveModule();
class CASModule;
extern std::stack<CASModule*> g_ModuleStack;
void CASCurrentModulePush(CASModule* pNewModule);

void CASCurrentModulePop();