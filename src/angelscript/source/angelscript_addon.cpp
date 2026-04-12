#include <svenenhancer.h>
#include <sqlite3_sven.h>

angelhook_t g_AngelHook;
void RegisterAngelScriptMethods() {
}
#define CREATE_AS_HOOK(item, des, tag, name, arg) g_AngelHook.item=ASEXT_RegisterHook(des,StopMode_CALL_ALL,2,ASFlag_MapScript|ASFlag_Plugin,tag,name,arg)

void RegisterAngelScriptHooks() {
}

void ShutDown()
{
}
void CloseAngelScriptsItem()
{
	sqliteKeepRun = false;
}
void ServerActivateAS()
{

}
void ServerDeactivateAS()
{

}

void ClearPlayerData() {

}

void ClearEntityData()
{
}

