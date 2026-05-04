#include <casmodule.h>
class SEFunction {
public:
	asIScriptContext* context;
	asIScriptModule* module;
	asIScriptFunction* function;
	CASModule* casmodule;
	asIScriptFunction* GetFunction()
	{
		return function;
	}
	asIScriptModule* GetModule()
	{
		return module;
	}
	CASModule* GetCasModule()
	{
		return casmodule;
	}
	asIScriptContext* GetContext()
	{
		return context;
	}
	void Release()
	{
		if (function)
			function->Release();
	}
	bool BeginCall()
	{
		if (!function)
			return false;
		CASCurrentModulePush(GetCasModule());
		return true;
	}
	void EndCall()
	{
		CASCurrentModulePop();
	}
	void AddRef()
	{
		function->AddRef();
	}
	static SEFunction* CreateFromFn(asIScriptFunction* function)
	{
		SEFunction* fn = new SEFunction();
		fn->function = function;
		fn->module = function->GetModule();
		fn->context = (asIScriptContext*) ASEXT_GetCurrentContext();
		fn->casmodule = ASEXT_GetServerManager()->curModule;
		return fn;
	}
};