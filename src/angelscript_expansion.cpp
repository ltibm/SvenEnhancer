#include <svenenhancer.h>
#include "angelscript_expansion.h"
#include <mysql_sven.h>
#include <callbackitem.h>

CASDocumentation* asDoc = nullptr;
SvenEnhancerAs* g_SE = nullptr;
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
		RegsiterObject<SvenEnhancerAs>("SvenEnhancer", engine);
		//engine->RegisterGlobalFunction("JValue@ JsonParse(string& in input)", asFunctionPtr(Json_Parse), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromFile(string& in path)", asFunctionPtr(Json_ParseFromFile), asCALL_CDECL);
		//engine->RegisterGlobalFunction("JValue@ JsonParseFromObject(ref @)", asFunctionPtr(Json_ParseObject), asCALL_CDECL);

		RegisterJValue(engine);

		RegisterMysqlAngelScript(engine);

		RegisterCallBackItem(pASDoc, engine);
		RegisterCallbackMethods(pASDoc, engine);

		engine->RegisterObjectMethod(
			"SvenEnhancer",
			"dictionary& get_globals()",
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


		//engine->RegisterObjectMethod("SvenEnhancer", "JValue@ JsonParseFromObject(ref @)", asMETHOD(SvenEnhancerAs, Json_ParseObject), asCALL_THISCALL);
		
		ASEXT_RegisterGlobalProperty(pASDoc, "Sven Enhancer", "SvenEnhancer@ SE", &g_SE);

	});
}



void Angelscript_ServerDeactivate()
{

}
void Angelscript_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
}
void AngelScript_Cleanup() {
	if (g_SE && g_SE->pDictionary)
	{

	}
}

