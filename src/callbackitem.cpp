#include <svenenhancer.h>
#include "CallbackItem.h"
CallbackItem::~CallbackItem()
{
	if (this->Data)
	{
		this->Data->Release();
		this->Data = nullptr;
	}
	if (this->Name)
	{
		this->Name->dtor();
		this->Name = nullptr;
	}
}
CallbackItem* CallbackItem::Factory() {
	CallbackItem* obj = new CallbackItem();
	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptEngine* engine = manager->scriptEngine;
	asITypeInfo* type = engine->GetTypeInfoByName("CallbackItem");
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
	reg = asFUNCTION(CallbackItem::Factory);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Factory", "CallbackItem", asBEHAVE_FACTORY, "CallbackItem@ CallbackItem()", &reg, asCALL_CDECL);
}
