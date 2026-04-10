#pragma once
void AngelScript_Expand();
void Angelscript_ServerDeactivate();
void Angelscript_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax);
void Angelscript_ClientDisconnect(edict_t * edict);
bool Angelscript_ClientCommand(edict_t* edict);
void Angelscript_ServerCommand(char* cmd);
void Angelscript_ServerExecute();
class asIScriptModule;
void Angelscript_OnModuleDiscard(asIScriptModule* module);
