Cvar@ cvar;
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );

	bool isRegistered = SE.RegisterClientCmd("mycommand", @CommandCallback, 999);
	isRegistered = SE.RegisterServerCmd("myservercmd", @ServerCallBack);
	//isRegistered = SE.RegisterClientCmd("mycommand", @CommandCallback, 998);
	
	@cvar = SE.RegisterCvar("my_cvar", "myvalue", FCVAR_NONE);

	if(@cvar !is null)
	{
		g_EngineFuncs.ServerPrint("Value: {v} \r\n".Fmt({{"v", cvar.GetString()}}));
	}
	//int c = FCVAR_NONE;

}  
int CommandCallback(edict_t@ edict, CallbackItem@ callback)
{
	//Prevent to call other CommandCallback
	callback.StopCall = true;
	int argc = g_EngineFuncs.Cmd_Argc();
	g_EngineFuncs.ServerPrint("Commadn Triggered With " + string(argc) + "\r\n");
	


	//Unregistered
	//SE.UnregisterClientCmd("mycommand", @CommandCallback);
	return 1;
}

void ServerCallBack(CallbackItem@ callback)
{
	int argc = g_EngineFuncs.Cmd_Argc();
	g_EngineFuncs.ServerPrint("Server Cmd Triggered With " + string(argc) + "\r\n");
	CallbackItem@ myItem  = CallbackItem();
	SE_EVENT.Trigger("command.executed", myItem);
	g_EngineFuncs.ServerPrint("Trigger Result: {code}\r\n".Fmt({{"code", myItem.ReturnCode}}));
}