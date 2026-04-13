RestClient@ client = RestClient();
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "steam:ibmlt" );
	g_Hooks.RegisterHook(Hooks::Player::ClientConnected, @ClientConnected);

}
void QueryUserCountry(const string &in sNick, const string &in sIp)
{
	string sip = sIp;
	int pos = sIp.FindFirstOf(":");
	if (pos != -1)
	{
		sip = sip.SubString(0, pos);
	}
	RestRequest@ request = RestRequest();
	request.Url = "http://ip-api.com/json/" + sip;
	//g_EngineFuncs.ServerPrint(request.Url);
	/*
		Although it may appear to be working within an anonymous method scope,
		the method is actually called externally, 
		so it cannot access variables outside of its own scope. For this reason, we pass the parameters we need using UserData.	
	*/
	@request.UserData = any(sNick);
	client.ExecuteAsync(@request, function(context){
		if(context.Response.StatusCode == 200)
		{	
			JValue@ json = context.Response.GetJsonBody();
			if(@json !is null)
			{
				//JValue@ path = json.Path("/country");
				//After v0.7 you can now use this
				string country = json.GetString("/country", true);
				if(country != "")
				{
					string nick;
					context.UserData.retrieve(nick);
					string message = SE.Interpolate("{nick} Connecting from [{country}]\n", {{"nick", nick}, {"country", country}});
					g_PlayerFuncs.ClientPrintAll(HUD_PRINTTALK, message);
					g_EngineFuncs.ServerPrint(message);
				}
			}
		}
	});
}
void PluginExit()
{

}
HookReturnCode ClientConnected(edict_t@ eEdict, const string &in sNick, const string &in sIp, bool &out bNoJoin, string &out sReason)
{
	QueryUserCountry(sNick, sIp);
	return HOOK_CONTINUE;
}
