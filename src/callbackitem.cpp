#include <svenenhancer.h>
#include "CallbackItem.h"
CallbackItem::CallbackItem()
{
	Name = CreateString("");
	ReturnString = *CreateString("");

}
CallbackItem::~CallbackItem()
{
	if (Data)
	{
		Data->Release();
		Data = nullptr;
	}
	if (Name)
	{
		Name->dtor();
		Name = nullptr;
	}
	ReturnString.dtor();
}

CallbackItem* CallbackItem::Factory() {
	CallbackItem* obj = new CallbackItem();
	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptContext* ctx = (asIScriptContext *) ASEXT_GetCurrentContext();
	if (ctx)
	{
		int ct = ctx->GetCallstackSize();
		if (ct >= 0)
		{
			auto fn = ctx->GetFunction(ct - 1);
			if (fn)
				obj->moduleName = fn->GetModuleName();
		}
	}

	asIScriptEngine* engine = manager->scriptEngine;
	static asITypeInfo* type  = nullptr;
	type = type ? type : engine->GetTypeInfoByName("CallbackItem");
	engine->NotifyGarbageCollectorOfNewObject(obj, type);
	return obj;
}

void RegisterCallBackItem(CASDocumentation* pASDoc, asIScriptEngine* engine)
{
	asSFuncPtr reg;
	ASEXT_RegisterObjectType(pASDoc, "Common CallbackItem", "CallbackItem", 0, asOBJ_REF | asOBJ_GC);
	RegisteGCObject<CallbackItem>(pASDoc, "CallbackItem");
	ASEXT_RegisterObjectProperty(pASDoc, "", "CallbackItem", "int ReturnCode", offsetof(CallbackItem, ReturnCode));
	ASEXT_RegisterObjectProperty(pASDoc, "", "CallbackItem", "any@ Data", offsetof(CallbackItem, Data));
	ASEXT_RegisterObjectProperty(pASDoc, "", "CallbackItem", "dictionary@ Dict", offsetof(CallbackItem, Dict));
	ASEXT_RegisterObjectProperty(pASDoc, "", "CallbackItem", "bool StopCall", offsetof(CallbackItem, StopCall));
	ASEXT_RegisterObjectProperty(pASDoc, "", "CallbackItem", "string ReturnString", offsetof(CallbackItem, ReturnString));
	engine->RegisterObjectMethod(
		"CallbackItem",
		"string& get_ModuleName()",
		asMETHOD(CallbackItem, GetModuleName),
		asCALL_THISCALL
	);
	reg = asFUNCTION(CallbackItem::Factory);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Factory", "CallbackItem", asBEHAVE_FACTORY, "CallbackItem@ CallbackItem()", &reg, asCALL_CDECL);
}
 