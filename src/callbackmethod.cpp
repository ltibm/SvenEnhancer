#include <callbackmethod.h>
#include <callbackitem.h>
void ClearModuleCallbacks(ModuleData* module)
{
	for (auto itorB = module->scriptCallbacks.begin(); itorB != module->scriptCallbacks.end();)
	{
		auto callbacks = itorB->second;
		for (auto itor = callbacks->begin(); itor != callbacks->end(); )
		{
			auto callback = itor->second;
			callback->Release();
			itor = callbacks->erase(itor);
		}
		itorB = module->scriptCallbacks.erase(itorB);
	}
	for (auto itor = module->methodCallbacks.begin(); itor != module->methodCallbacks.end();)
	{
		auto callback = itor->second;
		callback->Release();
		itor = module->methodCallbacks.erase(itor);
	}
}
void ClearAllModuleCallbacks()
{
	for (auto& module : g_ModuleData) {

		ClearModuleCallbacks(module);
	}
}

void CallOnReadyMethod(ModuleReadyData* data)
{
	if (data && data->function)
	{
		CASServerManager* manager = ASEXT_GetServerManager();
		asIScriptEngine* engine = manager->scriptEngine;
		asIScriptContext* ctx = engine->RequestContext();
		void* lastObj = nullptr;
		ctx->Prepare(data->function);
		int er = ctx->Execute();
		engine->ReturnContext(ctx);
	}
}

bool CheckOnReadyMethod(ModuleReadyData* data)
{
	if (!data)
		return false;
	for (const auto& key : data->moduleNames) {

		auto moduleDAta = findModuleByName(key);
		if (!moduleDAta || !moduleDAta->ready) {
			return false;
		}
	}
	CallOnReadyMethod(data);
	return true;
}
void CheckOnReadyMethods()
{
	for (auto itor = g_ModuleReadyData.begin(); itor != g_ModuleReadyData.end(); ) {
		auto data = *itor;
		if (!data || CheckOnReadyMethod(data)) {
			itor = g_ModuleReadyData.erase(itor);
		}
		else {
			++itor;
		}
	}
}

bool SC_SERVER_DECL OnModuleReady(void* pthis, SC_SERVER_DUMMYARG CScriptArray* array, asIScriptFunction* aslfn)
{

	if (!array || !aslfn || array->size() <= 0)
		return false;
	ModuleReadyData* data = new ModuleReadyData();
	for (size_t i = 0; i < array->size(); i++)
	{
		CString* strPtr = static_cast<CString*>(Array_At(array, i));
		data->moduleNames.push_back(strPtr->c_str());
	}
	data->function = aslfn;
	if (!CheckOnReadyMethod(data))
	{
		g_ModuleReadyData.push_back(data);
	}
	return true;
}
bool SC_SERVER_DECL ModuleReady(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	CASServerManager* manager = ASEXT_GetServerManager();
	auto module = GetModulePtr(manager);
	if (!module)
		return false;
	auto mName = module->GetName();
	auto moduleData = findModuleByModule(module);
	if (!moduleData || moduleData->ready)
		return false;
	moduleData->ready = true;
	CheckOnReadyMethods();
	return true;
}

bool SC_SERVER_DECL ModuleRegister(void* pthis, SC_SERVER_DUMMYARG CString* moduleName)
{
	if (!moduleName)
		return false;

	CASServerManager* manager = ASEXT_GetServerManager();
	const char* cmodule = moduleName->c_str();
	if (!manager->curModule)
		return false;
	auto module = GetModulePtr(manager);

	if (!module)
		return false;
	auto mName = module->GetName();

	//auto itor = findModuleByName(mName);
	auto itor = findModuleByModule(module);
	if (itor) {
		ClearModuleCallbacks(itor);
		itor->module = module;
		itor->moduleName = mName;
		itor->name = cmodule;
		itor->ready = false;
	}
	else {
		ModuleData* data = new ModuleData();
		data->module = module;
		data->moduleName = mName;
		data->name = cmodule;
		data->ready = false;
		g_ModuleData.push_back(data);
	}
	return true;
}

bool SC_SERVER_DECL RegisterMethod(void* pthis, SC_SERVER_DUMMYARG CString* name, asIScriptFunction* aslfn)
{
	if (!name || !aslfn)
		return false;

	auto mn = StructModuleName::fromStringDir(name->c_str());
	if (mn.module.empty())
		return false;

	auto module = findModuleByName(mn.module);
	if (!module)
		return false;

	auto moduleName = aslfn->GetModuleName();

	const char* cstr = name->c_str();

	auto exists = module->methodCallbacks.find(moduleName);
	if (exists != module->methodCallbacks.end())
	{
		exists->second->Release();
	}
	module->methodCallbacks[cstr] = aslfn;
	return true;
}

void* SC_SERVER_DECL CallMethod(void* pthis, SC_SERVER_DUMMYARG CString* name, void* any, void* dict)
{
	if (!name)
		return nullptr;

	std::string modStr = name->c_str();


	auto mn = StructModuleName::fromString(modStr);
	if (mn.module.empty())
		return nullptr;


	const char* cstr = mn.name.c_str();


	auto module = findModuleByName(mn.module);
	if (!module)
		return nullptr;

	auto itorA = module->methodCallbacks.find(cstr);

	if (itorA == module->methodCallbacks.end())
		return nullptr;

	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptEngine* engine = manager->scriptEngine;
	asIScriptContext* ctx = engine->RequestContext();

	//asIScriptObject* src =  any ? *static_cast<asIScriptObject**>(any) : nullptr;
	//asIScriptObject* srcd = dict ? *static_cast<asIScriptObject**>(dict) : nullptr;
	void* lastObj = nullptr;

	auto callback = itorA->second;
	if (!callback)
		return nullptr;
	ctx->Prepare(callback);
	CString* string = new CString();
	string->assign(cstr, strlen(cstr));
	ctx->SetArgObject(0, string);
	ctx->SetArgObject(1, any);
	ctx->SetArgObject(2, dict);

	int er = ctx->Execute();

	auto res = ctx->GetReturnObject();

	if (res)
	{
		CallbackItem* item = (CallbackItem*)res;
		item->AddRef();
		lastObj = item;
	}
	else {
		lastObj = nullptr;
	}
	engine->ReturnContext(ctx);

	string->dtor();
	return lastObj;
}


void RegisterCallbackMethods(CASDocumentation* pASDoc, asIScriptEngine* engine)
{

	ASEXT_RegisterFuncDef(pASDoc, "callback for Events", "void EventCallback(string&in name, CallbackItem@ item = null)");


	ASEXT_RegisterObjectMethod(pASDoc,
		"Register module for common", "SvenEnhancer", "bool Register(string&in moduleName)",
		(void*)ModuleRegister, asCALL_THISCALL);
	ASEXT_RegisterObjectMethod(pASDoc,
		"Ready module for common", "SvenEnhancer", "bool Ready()",
		(void*)ModuleReady, asCALL_THISCALL);

	ASEXT_RegisterFuncDef(pASDoc, "callback for RegisterMethod", "CallbackItem@ MethodCallback(string&in name, any@ data = null, dictionary@ dict = null)");

	ASEXT_RegisterFuncDef(pASDoc, "callback for Module Ready", "void OnModuleReadyHandler()");

	ASEXT_RegisterObjectMethod(pASDoc,
		"Module ready callback", "SvenEnhancer", "bool OnReady(array<string>&in modules, OnModuleReadyHandler@ callback)",
		(void*)OnModuleReady, asCALL_THISCALL);

	ASEXT_RegisterObjectMethod(pASDoc,
		"Register method for common", "SvenEnhancer", "bool RegisterMethod(string&in name, MethodCallback@ callback)",
		(void*)RegisterMethod, asCALL_THISCALL);

	ASEXT_RegisterObjectMethod(pASDoc,
		"Call method for common", "SvenEnhancer", "CallbackItem@ CallMethod(string&in name, any@ data = null, dictionary@ dict = null)",
		(void*)CallMethod, asCALL_THISCALL);

	engine->RegisterObjectMethod(
		"SvenEnhancerEvent",
		"bool On(string&in name, EventCallback@ item = null)",
		asMETHOD(SvenEnhancerEvent, On),
		asCALL_THISCALL
	);

	engine->RegisterObjectMethod(
		"SvenEnhancerEvent",
		"bool Off(string&in name, EventCallback@ item = null)",
		asMETHOD(SvenEnhancerEvent, Off),
		asCALL_THISCALL
	);

	engine->RegisterObjectMethod(
		"SvenEnhancerEvent",
		"uint Trigger(string&in name, CallbackItem@ item = null, bool callAll = false)",
		asMETHOD(SvenEnhancerEvent, Trigger),
		asCALL_THISCALL
	);
}
