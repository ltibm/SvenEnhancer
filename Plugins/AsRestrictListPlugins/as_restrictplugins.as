void PluginInit()
{
	//Sven Enhancer By S!
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "steam:ibmlt" );
	bool isRegistered = SE.RegisterClientCmd("as_listplugins", @CommandCallback, 999);

}  
int CommandCallback(edict_t@ edict, CallbackItem@ callback)
{
	//Prevent to call other CommandCallback
	callback.StopCall = true;
	int argc = g_EngineFuncs.Cmd_Argc();
	
	CBasePlayer@ pPlayer = g_PlayerFuncs.FindPlayerByIndex(g_EntityFuncs.EntIndex(edict));

    if (pPlayer is null)
        return 1;
	if(g_PlayerFuncs.AdminLevel(@pPlayer) == ADMIN_OWNER)
		return 0;

	//Unregistered
	g_PlayerFuncs.ClientPrint(pPlayer, HUD_PRINTCONSOLE, "as_listplugins is restricted\n");
	return 1;
}
