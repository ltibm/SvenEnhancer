#include <vector>
#include <map>

#include <extdll.h>
#include <meta_api.h>


#include "signatures.h"
#include "asext_api.h"

#include "angelscript_addon.h"

#pragma region Function Def
#pragma endregion

#pragma region SigHooks


bool InstallEngineHook() {
	auto ServerHandle = gpMetaUtilFuncs->pfnGetGameDllHandle();
	auto EngineHandle = gpMetaUtilFuncs->pfnGetEngineHandle();

	auto ServerBase = gpMetaUtilFuncs->pfnGetGameDllBase();
	auto EngineBase = gpMetaUtilFuncs->pfnGetEngineBase();

	auto ServerSize = gpMetaUtilFuncs->pfnGetImageSize(ServerBase);
	return true;
}
void UninstallEngineHook() {
}
#pragma endregion
