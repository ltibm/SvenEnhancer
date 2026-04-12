#include <svenenhancer.h>
#include "angelscript_expansion.h"
#include <mysql_sven.h>
#include <callbackitem.h>
#include <sqlite3_sven.h>

CASDocumentation* asDoc = nullptr;
SvenEnhancerAs* g_SE = nullptr;
//SvenEnhancerEnt* g_SEEnt = nullptr;
SvenEnhancerEnt g_SEEnt;
SvenEnhancerEvent g_SEEvent;

typedef void (AS_CALL* DiscardFn)(asIScriptModule* pThis);
DiscardFn oDiscard = nullptr;
static bool is_hooking = true;
void AS_CALL hookDiscard(asIScriptModule* pThis) {
	//Now we can access plugin unload

	if (oDiscard)
		oDiscard(pThis);
	if (!is_hooking && pThis)
		Angelscript_OnModuleDiscard(pThis);
}
static CString* String_Interpolate(CString& str, void* dict)
{
	return g_SE->Interpolate(str, (void*) dict);
}
void AngelScript_Expand() {

	ASEXT_RegisterScriptBuilderDefineCallback([](CScriptBuilder* pScriptBuilder) {
		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "SE");
		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "SE_MYSQL");
	});

	ASEXT_RegisterDocInitCallback([](CASDocumentation* pASDoc) {
		g_SE = new SvenEnhancerAs();
		asDoc = pASDoc;
		asIScriptEngine* engine = GetASEngine();
		
		//Hook module Discard for detecting plugin unload
		auto tempModule = engine->GetModule("TEMP_MODULE_SE_123", asGM_ALWAYS_CREATE);
		if (tempModule) {
			void** vtable = *(void***)tempModule;
			//3= Discard();
			oDiscard = (DiscardFn)vtable[3];
			unsigned long oldProt;
			if (SetMemoryWritable(&vtable[3], sizeof(void*), true, oldProt)) {
				vtable[3] = (void*)(uintptr_t)hookDiscard;
				SetMemoryWritable(&vtable[3], sizeof(void*), false, oldProt);
			}
			//Remove temporary module
			tempModule->Discard();
			is_hooking = false;
		}

		InitializeGlobalTypes(engine);
		//RegsiterObject<SvenEnhancerAs>("Sven
		// Enhancer", engine, asOBJ_REF | asOBJ_NOCOUNT);
		//RegsiterObject<SvenEnhancerEnt>("SvenEnhancerEntity", engine);
		engine->RegisterObjectType("SvenEnhancer", 0, asOBJ_REF | asOBJ_NOCOUNT);
		engine->RegisterObjectType("SvenEnhancerEntity", 0, asOBJ_REF | asOBJ_NOCOUNT);
		engine->RegisterObjectType("SvenEnhancerEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
		//engine->RegisterGlobalFunction("JValue@ JsonParse(string& in input)", asFunctionPtr(Json_Parse), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromFile(string& in path)", asFunctionPtr(Json_ParseFromFile), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromObject(ref @)", asFunctionPtr(Json_ParseObject), asCALL_CDECL);

		RegisterJValue(engine);
		RegisterMysqlAngelScript(engine);
		RegisterSqliteAngelScript(engine);
		//Register RestClient
		RestClient::RegisterAngelScript(pASDoc, engine);
		RegisterCallBackItem(pASDoc, engine);
		RegisterCallbackMethods(pASDoc, engine);

		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"dictionary@+ get_globals()",
			asMETHOD(SvenEnhancerAs, getGlobals),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod("SvenEnhancer", "string& Interpolate(string& in input, dictionary@ params = null) const", asMETHOD(SvenEnhancerAs, Interpolate), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "string& Fmt(string& in input, dictionary@ params = null) const", asMETHOD(SvenEnhancerAs, Interpolate), asCALL_THISCALL);

		engine->RegisterObjectMethod(
			"string",
			"string& Fmt(dictionary@ params = null) const",
			asFUNCTION(String_Interpolate),
			asCALL_CDECL_OBJFIRST
		);
		engine->RegisterObjectMethod("SvenEnhancer", "string& get_VERSION() const", asMETHOD(SvenEnhancerAs, Version), asCALL_THISCALL);

		engine->RegisterObjectMethod("SvenEnhancer", "JValue@ JsonParse(string& in input)", asMETHOD(SvenEnhancerAs, Json_Parse), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "JValue@ JsonParseFromFile(string& in path)", asMETHOD(SvenEnhancerAs, Json_ParseFromFile), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "JValue@ JsonParseFromObject(?&in obj)", asMETHOD(SvenEnhancerAs, Json_ParseObjectV2), asCALL_THISCALL);

		engine->RegisterObjectMethod("SvenEnhancer", "bool MySql_Loaded()", asMETHOD(SvenEnhancerAs, MySql_Loaded), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "MySqlConnection@ MySql_CreateConnection(MySqlConnectionConfig& in config)", asMETHOD(SvenEnhancerAs, MySqlConnection_Create), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "void PluginExit()", asMETHOD(SvenEnhancerAs, PluginExit), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "string& HMAC_SHA1(string& in key, string& in data) const", asMETHOD(SvenEnhancerAs, HMAC_SHA1AS), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "string& HMAC_SHA256(string& in key, string& in data) const", asMETHOD(SvenEnhancerAs, HMAC_SHA256AS), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "string& MD5(string& in data) const", asMETHOD(SvenEnhancerAs, MD5AS), asCALL_THISCALL);

		engine->RegisterObjectMethod("SvenEnhancer", "string& Base64_Encode(string& in data) const", asMETHOD(SvenEnhancerAs, BASE64Encode), asCALL_THISCALL);
		engine->RegisterObjectMethod("SvenEnhancer", "string& Base64_Decode(string& in data) const", asMETHOD(SvenEnhancerAs, BASE64Decode), asCALL_THISCALL);

		//engine->RegisterObjectMethod("SvenEnhancer", "JValue@ JsonParseFromObject(ref @)", asMETHOD(SvenEnhancerAs, Json_ParseObject), asCALL_THISCALL);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataInt(CBaseEntity@ ent, int offset, int value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataInt),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"int GetPDataInt(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataInt),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataUInt(CBaseEntity@ ent, int offset, uint value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataUInt),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"uint GetPDataUInt(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataUInt),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataBool(CBaseEntity@ ent, int offset, bool value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataBool),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool GetPDataBool(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataBool),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataFloat(CBaseEntity@ ent, int offset, float value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataFloat),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"float GetPDataFloat(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataFloat),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataShort(CBaseEntity@ ent, int offset, int16 value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataShort),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"int16 GetPDataShort(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataShort),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"bool SetPDataUShort(CBaseEntity@ ent, int offset, uint16 value, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, SetPDataUShort),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"uint16 GetPDataUShort(CBaseEntity@ ent, int offset, int linuxdiff = 12)",
			asMETHOD(SvenEnhancerEnt, GetPDataUShort),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"dictionary@+ GetData(CBaseEntity@ entity)",
			asMETHOD(SvenEnhancerEnt, GetDataByEntity),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"dictionary@+ GetData(edict_t@ edict)",
			asMETHOD(SvenEnhancerEnt, GetDataByEdict),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancerEntity",
			"dictionary@+ GetData(int index)",
			asMETHOD(SvenEnhancerEnt, GetData),
			asCALL_THISCALL
		);


		ASEXT_RegisterFuncDef(pASDoc, "callback for ClientCmd", "int ClientCmdCallback(edict_t@ edict, CallbackItem@ item = null)");
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool RegisterClientCmd(string&in cmd, ClientCmdCallback@ callback, int order = 9999)",
			asMETHOD(SvenEnhancerAs, RegisterClientCmd),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool UnregisterClientCmd(string&in cmd, ClientCmdCallback@ callback)",
			asMETHOD(SvenEnhancerAs, UnregisterClientCmd),
			asCALL_THISCALL
		);
		ASEXT_RegisterFuncDef(pASDoc, "callback for ServerCmd", "void ServerCmdCallback(CallbackItem@ item = null)");

		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool RegisterServerCmd(string&in cmd, ServerCmdCallback@ callback, int order = 9999)",
			asMETHOD(SvenEnhancerAs, RegisterServerCmd),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool UnregisterServerCmd(string&in cmd, ServerCmdCallback@ callback)",
			asMETHOD(SvenEnhancerAs, UnregisterServerCmd),
			asCALL_THISCALL
		);

		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool ClientCmd(edict_t@ edict, string&in cmd)",
			asMETHOD(SvenEnhancerAs, ClientCmd),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"bool ClientCmd(uint index, string&in cmd)",
			asMETHOD(SvenEnhancerAs, ClientCmdI),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"string& SQLiteEscape(string&in input)",
			asMETHOD(SvenEnhancerAs, SqliteEscape),
			asCALL_THISCALL
		);
		engine->RegisterEnumValue("FCVAR", "FCVAR_NONE", 0);

		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"Cvar@ RegisterCvar(string&in name, string&in value=\"\", FCVAR flags = FCVAR_NONE, float flValue = 0)",
			asMETHOD(SvenEnhancerAs, RegisterCvar),
			asCALL_THISCALL
		);
		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"Cvar@ GetCvar(string&in name)",
			asMETHOD(SvenEnhancerAs, GetCvar),
			asCALL_THISCALL
		);
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer", "SvenEnhancer@ SE", &g_SE);
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer Entity", "SvenEnhancerEntity SE_ENT", &g_SEEnt);
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer Event", "SvenEnhancerEvent SE_EVENT", &g_SEEvent);
		});
}
static std::unordered_set<std::string> ignored_clcmds = {

};
bool Angelscript_ClientCommand(edict_t* edict) {
	std::string cmd = CMD_ARGV(0);
	if (ignored_clcmds.contains(cmd))
	{
		return false;
	}
	int r = g_SE->TriggerClientCmd(edict, cmd);
	if (r)
	{
		SET_META_RESULT(MRES_SUPERCEDE);
		return true;
	}
	return false;

}
void Angelscript_ServerExecute() {
	auto cmd = CMD_ARGS();
	SET_META_RESULT(MRES_IGNORED);

}

void Angelscript_ServerCommand(char* cmd) {
	SET_META_RESULT(MRES_IGNORED);
}
//Map End
void Angelscript_ServerDeactivate()
{
	//Release all entity data
	g_SEEnt.ClearAllEntityData();
}

void Angelscript_ClientDisconnect(edict_t* edict) {
	if (!edict || !edict->pvPrivateData)
		return;
	//Clear player data
	g_SEEnt.ClearPlayerData(ENTINDEX(edict));
}

//Map Start
void Angelscript_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
}

void Angelscript_OnModuleDiscard(asIScriptModule* module)
{
	g_SE->PluginExitIntl(module);
}
void AngelScript_Cleanup() {
	if (g_SE && g_SE->pDictionary)
	{

	}
}

