#pragma once
#include <svenenhancer.h>
static std::unordered_map<int, std::string> g_userinfoCache;
inline int Hook_ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	auto adminState = GetAdminState(pEntity);
	int id = ENTINDEX(pEntity);
	if (id >= 0 && id <= 32)
	{
		VoiceState& voiceState = g_SE->g_VoiceState[id];
		voiceState.isMuted = false;
		voiceState.teamId = 0;
		voiceState.isAdmin = adminState.isAdmin || adminState.isOwner;
		voiceState.level = 0;
	}
	SET_META_RESULT(MRES_IGNORED);
	return 1;
}
inline void Hook_CvarSetFloat(const char* name, float value)
{
	SET_META_RESULT(MRES_IGNORED);

}
inline void Hook_CvarSetString(const char* name, const char* value)
{
	SET_META_RESULT(MRES_IGNORED);
}
inline void Hook_CvarDirectSet(struct cvar_s* var, char* value) {

	SET_META_RESULT(MRES_IGNORED);
}
inline void Hook_ClientDisconnect(edict_t* pEntity)
{
	int id = ENTINDEX(pEntity);
	g_userinfoCache.erase(id);
	if (id >= 0 && id <= 32)
	{
		VoiceState& voiceState = g_SE->g_VoiceState[id];
		voiceState.isMuted = false;
		voiceState.teamId = 0;
		voiceState.isAdmin = false;
		voiceState.level = 0;
	}
}
inline void Hook_CvarValue(const edict_t* pEnt, const char* value)
{
	auto key = MAKE_EVENTKEY(EVENT_PLAYERQUERYCVAR, 0);
	CallbackItem* item = new CallbackItem();
	std::vector<DynamicArg> args;
	DynamicArg arg;
	arg.type = ArgType::Object;
	arg.objVal =(void*) pEnt;
	args.push_back(arg);

	DynamicArg arg2;
	arg2.type = ArgType::String;
	arg2.strVal = value;
	args.push_back(arg2);

	g_SEEvent.TriggerGameEvent(key, item, args);
	SET_META_RESULT(MRES_IGNORED);
}
inline void Hook_CvarValue2(const edict_t* pEnt, int requestID, const char* cvarName, const char* value)
{
	auto key = MAKE_EVENTKEY(EVENT_PLAYERQUERYCVAR2, 0);
	CallbackItem* item = new CallbackItem();
	std::vector<DynamicArg> args;
	DynamicArg arg;
	arg.type = ArgType::Object;
	arg.objVal = (void*)pEnt;
	args.push_back(arg);

	DynamicArg arg2;
	arg2.type = ArgType::Int;
	arg2.intVal = requestID;
	args.push_back(arg2);

	DynamicArg arg3;
	arg3.type = ArgType::String;
	arg3.strVal = cvarName;
	args.push_back(arg3);

	DynamicArg arg4;
	arg3.type = ArgType::String;
	arg3.strVal = value;
	args.push_back(arg3);

	g_SEEvent.TriggerGameEvent(key, item, args);
	SET_META_RESULT(MRES_IGNORED);
}
inline qboolean Voice_SetClientListening(int receiver, int sender, qboolean listen)
{
	if(g_SE->voice_Disabled)
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);
	if (sender < 1 || sender > 32 || receiver < 1 || receiver > 32)
		RETURN_META_VALUE(MRES_IGNORED, listen);
	VoiceState& senderState = g_SE->g_VoiceState[sender];
	VoiceState& receiverState = g_SE->g_VoiceState[receiver];
	if (senderState.isMuted)
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);
	if (senderState.teamId != receiverState.teamId)
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);
	if (g_SE->voice_AdminOnly && senderState.isAdmin)
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);
	if(g_SE->voice_MinLevel > 0 && !senderState.isAdmin && senderState.level < g_SE->voice_MinLevel)
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);

	RETURN_META_VALUE(MRES_IGNORED, listen);
}
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