Cvar@ cvar_GameDesc;
void PluginInit()
{
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "steam:ibmlt" );
	@cvar_GameDesc = SE.RegisterCvar("se_gamedescription", "", FCVAR_NONE);
	SE_EVENT.OnGame("getgamedescription", @HookGetGameDesc);
}
void HookGetGameDesc(CallbackItem@ item)
{
	string cvarStr = cvar_GameDesc.GetString();
	if(cvarStr != "")
	{
		item.ReturnCode = 1;
		item.ReturnString = cvarStr;
	}
}