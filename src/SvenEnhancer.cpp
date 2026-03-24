#include <svenenhancer.h>
#include <angelscript_expansion.h>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
IMPORT_ASEXT_API_DEFINE()



static META_FUNCTIONS gMetaFunctionTable = {
	NULL,			// pfnGetEntityAPI				HL SDK; called before game DLL
	NULL,			// pfnGetEntityAPI_Post			META; called after game DLL
	GetEntityAPI2,	// pfnGetEntityAPI2				HL SDK2; called before game DLL
	GetEntityAPI2_Post,			// pfnGetEntityAPI2_Post		META; called after game DLL
	NULL,			// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
	NULL,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
	GetEngineFunctions,	// pfnGetEngineFunctions	META; called before HL engine
	NULL,			// pfnGetEngineFunctions_Post	META; called after HL engine
	NULL,			// pfnGetStudioBlendingInterface 2022/07/02 Added by hzqst
	NULL,			// pfnGetStudioBlendingInterface_Post 2022/07/02 Added by hzqst
};



// Plugin info
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"SvenEnhancer",	// name
	SE_VERSION,	// version
	SE_DATE,	// date
	"S!",	// author
	"https://github.com/ibm-p/SvenEnhancer-metamod",	// url
	"SvenEnhancer",	// logtag, all caps please
	PT_ANYTIME,	// (when) loadable
	PT_STARTUP,	// (when) unloadable
};


// Global vars from metamod:
meta_globals_t* gpMetaGlobals;		// metamod globals
gamedll_funcs_t* gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t* gpMetaUtilFuncs;		// metamod utility functions

// Metamod requesting info about this plugin:
//  ifvers			(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod



extern "C" {

	C_DLLEXPORT int Meta_Query(const char* interfaceVersion, plugin_info_t** pPlugInfo, mutil_funcs_t* pMetaUtilFuncs)
	{
		if (0 != strcmp(interfaceVersion, META_INTERFACE_VERSION))
		{
			pMetaUtilFuncs->pfnLogError(PLID, "Meta_Query version mismatch! expect %s but got %s", META_INTERFACE_VERSION, interfaceVersion);
			return FALSE;
		}

		// Give metamod our plugin_info struct
		*pPlugInfo = &Plugin_info;
		// Get metamod utility function table.
		gpMetaUtilFuncs = pMetaUtilFuncs;
		return TRUE;
	}

	C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME /* now */,
		META_FUNCTIONS* pFunctionTable, meta_globals_t* pMGlobals,
		gamedll_funcs_t* pGamedllFuncs)
	{		if (!pMGlobals) {
			LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
			return FALSE;
		}

		gpMetaGlobals = pMGlobals;

		if (!pFunctionTable) {
			LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
			return FALSE;
		}

		memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));

		gpGamedllFuncs = pGamedllFuncs;

		auto engineHandle = gpMetaUtilFuncs->pfnGetEngineHandle();
		auto engineBase = gpMetaUtilFuncs->pfnGetEngineBase();

		if (!engineHandle)
		{
			LOG_ERROR(PLID, "engine handle not found!");
			return FALSE;
		}

		if (!engineBase)
		{
			LOG_ERROR(PLID, "engine base not found!");
			return FALSE;
		}

		void* asextHandle = nullptr;
#ifdef _WIN32
		LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.dll", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#else
		LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.so", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#endif
		if (!asextHandle)
		{
			LOG_ERROR(PLID, "asext dll handle not found!");
			return FALSE;
		}
		IMPORT_ASEXT_API(asext);
		RegisterAngelScriptMethods();
		AngelScript_Expand();
		RegisterAngelScriptHooks();
		return TRUE;
	}

	// Metamod detaching plugin from the server.
	// now		(given) current phase, ie during map, etc
	// reason	(given) why detaching (refresh, console unload, forced unload, etc)
	C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME /* now */,
		PL_UNLOAD_REASON /* reason */) {
		UninstallEngineHook();
		//VtableUnhook();
		CloseAngelScriptsItem();
		return TRUE;
	}

}