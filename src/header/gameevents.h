#pragma once
#include <svenenhancer.h>

inline const char* Hook_GetGameDescription()
{
	auto key = MAKE_EVENTKEY(EVENT_GETGAMEDESCRIPTION, 0);
	CallbackItem* item = new CallbackItem();
	g_SEEvent.TriggerGameEvent(key, item);
	if (item->ReturnCode == 1)
	{
		static std::string ret = item->ReturnString.c_str();
		SET_META_RESULT(MRES_SUPERCEDE);
		return ret.c_str();
	}
	//fix
	SET_META_RESULT(MRES_IGNORED);
	return {};
}
inline void Hook_KeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd)
{
	auto key = MAKE_EVENTKEY(EVENT_KEYVALUE, 0);
	if (!pkvd || !pkvd->szKeyName || !pkvd->szValue)
	{
		SET_META_RESULT(MRES_IGNORED);
		return;
	}
	CallbackItem* item = new CallbackItem();
	std::vector<DynamicArg> args;
	DynamicArg arg;
	arg.type = ArgType::Object;
	arg.objVal = pentKeyvalue;
	args.push_back(arg);


	DynamicArg argc;
	argc.type = ArgType::Object;
	argc.objVal = pkvd;
	args.push_back(argc);
	g_SEEvent.TriggerGameEvent(key, item, args);
	if (item->ReturnCode == 1)
	{
		SET_META_RESULT(MRES_SUPERCEDE);
		return;
	}
	SET_META_RESULT(MRES_IGNORED);
}
static std::unordered_map<int, std::string> g_userinfoCache;
inline void Hook_ClientUserInfoChanged(edict_t* edict, char* infobuffer)
{
	auto key = MAKE_EVENTKEY(EVENT_CLIENTUSERINFOCHANGED, 0);
	CallbackItem* item = new CallbackItem();
	int id = ENTINDEX(edict);

	std::string newInfo = infobuffer ? infobuffer : "";
	std::string oldInfo = g_userinfoCache[id];
	g_userinfoCache[id] = newInfo;
	if (oldInfo != newInfo && !oldInfo.empty())
	{

		std::vector<DynamicArg> args;

		DynamicArg argo;
		argo.type = ArgType::Object;
		argo.objVal = edict;
		args.push_back(argo);

		DynamicArg arg;
		arg.type = ArgType::String;
		std::string str{};
		str.assign(oldInfo.c_str(), oldInfo.size());
		arg.strVal = str;
		args.push_back(arg);

		DynamicArg arg2;
		arg2.type = ArgType::String;
		std::string str2{};
		str2.assign(newInfo.c_str(), newInfo.size());
		arg2.strVal = str2;
		args.push_back(arg2);

		g_SEEvent.TriggerGameEvent(key, item, args);
		if (item->ReturnCode == 1)
		{
			SET_META_RESULT(MRES_SUPERCEDE);
		}
	}
	SET_META_RESULT(MRES_IGNORED);
}
inline void Hook_ClientDisconnect(edict_t* pEntity)
{
	int id = ENTINDEX(pEntity);
	g_userinfoCache.erase(id);
}
inline CString* KVD_ClassName(KeyValueData* pThis)
{
	return CreateString(pThis->szClassName);
}
inline void KVD_SetClassName(KeyValueData* pThis, CString& value)
{
	std::string str = value.c_str();
	char* newStr = AS_STRDUP(str.c_str());
	free(pThis->szClassName);
	pThis->szClassName = newStr;
}
inline CString* KVD_KeyName(KeyValueData* pThis)
{
	return CreateString(pThis->szKeyName);
}
inline void KVD_SetKeyName(KeyValueData* pThis, CString& value)
{
	std::string str = value.c_str();
	char* newStr = AS_STRDUP(str.c_str());
	free(pThis->szKeyName);
	pThis->szKeyName = newStr;
}
inline void KVD_SetKeyValue(KeyValueData* pThis, CString& value)
{
	std::string str = value.c_str();
	char* newStr = AS_STRDUP(str.c_str());
	free(pThis->szValue);
	pThis->szValue = newStr;
}
inline CString* KVD_KeyValue(KeyValueData* pThis)
{	
	return CreateString(pThis->szValue);
}
inline void RegisterSEEventsContext(CASDocumentation* pASDoc, asIScriptEngine* engine) {
	engine->RegisterObjectType("SEKeyValueData", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("SEKeyValueData", "string& get_ClassName()", asFunctionPtr(&KVD_ClassName), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("SEKeyValueData", "void set_ClassName(const string&in)", asFunctionPtr(&KVD_SetClassName), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("SEKeyValueData", "string& get_KeyName()", asFunctionPtr(&KVD_KeyName), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("SEKeyValueData", "void set_KeyName(const string&in)", asFunctionPtr(&KVD_SetKeyName), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("SEKeyValueData", "string& get_KeyValue()", asFunctionPtr(&KVD_KeyValue), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("SEKeyValueData", "void set_KeyValue(const string&in)", asFunctionPtr(&KVD_SetKeyValue), asCALL_CDECL_OBJFIRST);


	engine->RegisterObjectProperty("SEKeyValueData", "bool Handled", offsetof(KeyValueData, fHandled), asCALL_THISCALL);



}