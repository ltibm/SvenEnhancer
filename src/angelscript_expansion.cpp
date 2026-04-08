#include <svenenhancer.h>
#include "angelscript_expansion.h"
#include <mysql_sven.h>
#include <callbackitem.h>

CASDocumentation* asDoc = nullptr;
SvenEnhancerAs* g_SE = nullptr;
//SvenEnhancerEnt* g_SEEnt = nullptr;
SvenEnhancerEnt g_SEEnt;

void AngelScript_Expand() {

	ASEXT_RegisterScriptBuilderDefineCallback([](CScriptBuilder* pScriptBuilder) {
		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "SE");
		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "SE_MYSQL");
	});

	ASEXT_RegisterDocInitCallback([](CASDocumentation* pASDoc) {
		g_SE = new SvenEnhancerAs();
		asDoc = pASDoc;
		asIScriptEngine* engine = GetASEngine();
		InitializeGlobalTypes(engine);
		//RegsiterObject<SvenEnhancerAs>("SvenEnhancer", engine, asOBJ_REF | asOBJ_NOCOUNT);
		//RegsiterObject<SvenEnhancerEnt>("SvenEnhancerEntity", engine);
		engine->RegisterObjectType("SvenEnhancer", 0, asOBJ_REF | asOBJ_NOCOUNT);
		engine->RegisterObjectType("SvenEnhancerEntity", 0, asOBJ_REF | asOBJ_NOCOUNT);
		//engine->RegisterGlobalFunction("JValue@ JsonParse(string& in input)", asFunctionPtr(Json_Parse), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromFile(string& in path)", asFunctionPtr(Json_ParseFromFile), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromObject(ref @)", asFunctionPtr(Json_ParseObject), asCALL_CDECL);

		RegisterJValue(engine);

		RegisterMysqlAngelScript(engine);
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
		engine->RegisterObjectMethod("SvenEnhancer", "string& Interpolate(string& in input, dictionary@ params) const", asMETHOD(SvenEnhancerAs, Interpolate), asCALL_THISCALL);
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
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer", "SvenEnhancer@ SE", &g_SE);
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer Entity", "SvenEnhancerEntity SE_ENT", &g_SEEnt);
	});
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
void AngelScript_Cleanup() {
	if (g_SE && g_SE->pDictionary)
	{

	}
}

