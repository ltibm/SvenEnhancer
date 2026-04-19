RestClient@ client = RestClient();
CTextMenu@ menu_whereFrom;
CTextMenu@ menu_whereFromDetails;
class IpInfo
{
	bool Pending = false;
	string Country;
	string CountryCode;
	string As;
	string Error;
	string IpAddress;
}

uint GetPage(int cid, int perpageitems = 7)
{
	float msonuc = floor(float(cid) / float(perpageitems));
	return uint(msonuc);
}
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "steam:ibmlt" );
	g_Hooks.RegisterHook(Hooks::Player::ClientConnected, @ClientConnected);
	g_Hooks.RegisterHook(Hooks::Player::ClientSay, @ClientSay);
	SE.RegisterServerCmd("wherefrom", @WhoIsCallback);
}
void PluginExit()
{
	if (@menu_whereFrom !is null)
	{
		if(menu_whereFrom.IsRegistered())
			menu_whereFrom.Unregister();
		@menu_whereFrom = null;
	}
	if (@menu_whereFromDetails !is null)
	{
		if(menu_whereFromDetails.IsRegistered())
			menu_whereFromDetails.Unregister();
		@menu_whereFromDetails = null;
	}
}
void MapInit()
{
	
}
HookReturnCode ClientSay(SayParameters@ sayitem)
{
    CBasePlayer@ pPlayer = sayitem.GetPlayer();
    if (pPlayer is null)
        return HOOK_CONTINUE;
	const CCommand@ args = sayitem.GetArguments();
	string arg = args[0];
    if (arg == "!wherefrom" || arg == '!kimnerden')
    {
		sayitem.ShouldHide = true;
		ShowWhereFromMenu(@pPlayer);
        return HOOK_HANDLED;
    }
    return HOOK_CONTINUE;
}
void ShowWhereFromMenu(CBasePlayer@ pPlayer, uint page = 0)
{
	if(g_Engine.maxClients == 0)
		return;
	@menu_whereFrom =  CTextMenu(@WhereFromMenuCallback);
	for(uint i = 1; i < g_Engine.maxClients; i++)
	{
		CBasePlayer@ player = g_PlayerFuncs.FindPlayerByIndex(i);
		if(player is null || !player.IsConnected())
			continue;
		dictionary@ data = player.GetData();
		if(data is null)
			return;
		IpInfo info;
		if(!data.get("IpInfo", info))
			continue;
		string prefix;
		string user = string(player.pev.netname);
		if (info.Pending)
			prefix = "..";
		else if (info.Error.Length() > 0)
			prefix = "?";
		else
			prefix = info.CountryCode;
		menu_whereFrom.AddItem(SE.Sprintf("\\y[%s]\\w%s", prefix, user), any(EHandle(player)));
	}
	menu_whereFrom.SetTitle("Where From");
	menu_whereFrom.Register();
	uint maxp = menu_whereFrom.GetPageCount() - 1;
	if(page > maxp) page = maxp;
	menu_whereFrom.Open(0, page, @pPlayer);
}
void ShowWhereFromMenuDetail(CBasePlayer@ pPlayer, CBasePlayer@ tPlayer, uint page)
{
	if(pPlayer is null || tPlayer is null || !pPlayer.IsConnected() || !tPlayer.IsConnected())
		return;
	dictionary@ data = tPlayer.GetData();
	IpInfo info;
	if(data is null || !data.get("IpInfo", info))
		return;
	@menu_whereFromDetails = CTextMenu(@WhereFromMenuDetailsCallback);
	bool isAdmin =  g_PlayerFuncs.AdminLevel(@pPlayer) > ADMIN_NO;
	string user = string(tPlayer.pev.netname);

	string userInfo = "Where From: " + user;
	string countryText = "";
	if(info.Pending)
	{
		countryText= "Loading...";
	}
	else if(info.Error.Length() > 0)
	{
		countryText = info.Error;
	}
	else{
		countryText = SE.Sprintf("[%s]%s", info.CountryCode, info.Country);
	}
	userInfo += SE.Sprintf("\nCountry: %s", countryText);
	if(isAdmin)
	{
		userInfo += SE.Sprintf("\nIp: %s", info.IpAddress);
		if(info.As.Length() > 0)
			userInfo += SE.Sprintf("\nISP: %s", info.As);
	}
	menu_whereFromDetails.SetTitle(userInfo);
	menu_whereFromDetails.AddItem("Back", any(page));
	menu_whereFromDetails.Register();
	menu_whereFromDetails.Open(0, 0, @pPlayer);

}
void WhereFromMenuDetailsCallback(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)
{
	if(pItem is null)
	{
		menu.Unregister();
		return;
	}
	uint page;
	pItem.m_pUserData.retrieve(page);
	ShowWhereFromMenu(@pPlayer, page);
}
void WhereFromMenuCallback(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)
{ 
	if(pItem is null)
	{
		menu.Unregister();
		return;
	}
	uint page = GetPage(iSlot);
	EHandle target;
	pItem.m_pUserData.retrieve(target);
	menu.Unregister();
	if(target.IsValid())
	{
		CBasePlayer@ tPlayer = cast<CBasePlayer@>(target.GetEntity());
		ShowWhereFromMenuDetail(@pPlayer, @tPlayer, page);
	}

}
void WhoIsCallback(CallbackItem@ callback)
{
	PrintIpInfo();
}
void PrintIpInfo()
{
	for(uint i = 1; i < g_Engine.maxClients; i++)
	{
		CBasePlayer@ player = g_PlayerFuncs.FindPlayerByIndex(i);
		if(player is null || !player.IsConnected())
			continue;
		dictionary@ data = player.GetData();
		if(data is null)
			return;
		IpInfo info;
		if(!data.get("IpInfo", info))
			continue;
		string status;
		string user = string(player.pev.netname);
		if (info.Pending)
			status = "Loading...";
		else if (info.Error.Length() > 0)
			status = info.Error;
		else
			status = info.Country + " " + info.As;
		
		SE.Printf("%-15s %-20s %-25s\n",
			user,
			info.IpAddress,
			status
		);
	}
}
void QueryUserCountry(edict_t@ eEdict, const string &in sNick, const string &in sIp)
{
	dictionary@ data = eEdict.GetData();
	if(data is null)
		return;
	IpInfo info;
	if(data.get("IpInfo", info))
	{
		return;
	}
	else
	{
		info.Pending = true;
		info.IpAddress = sIp;
		data["IpInfo"] = info;
	}			

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
	@request.UserData = any(dictionary = {{"nick", sNick}, {"entity", @eEdict}});
	client.ExecuteAsync(@request, function(context){
		if(context.Response.StatusCode == 200)
		{	
			JValue@ json = context.Response.GetJsonBody();
			if(@json !is null)
			{
				//JValue@ path = json.Path("/country");
				//After v0.7 you can now use this
				string status = json.GetString("/status", true);
				dictionary params;
				if(context.UserData.retrieve(params))
				{
						string nick;
						params.get("nick", nick);
						edict_t@ eEdict;
						params.get("entity", @eEdict);
						if(eEdict !is null && eEdict.free == 0)
						{
						
							dictionary@ data = eEdict.GetData();
							IpInfo info;
							if(!data.get("IpInfo", info))
								return;
							info.Pending = false;
							if(status == "fail")
							{
								info.Error = json.GetString("/message", true);
							
							}
							else if(status == "success")
							{
								info.Country = json.GetString("/country", true);
								info.CountryCode = json.GetString("/countryCode", true);
								info.As = json.GetString("/as", true);
								string message = SE.Interpolate("{nick} Connecting from [{country}]\n", {{"nick", nick}, {"country", info.Country}});
								g_PlayerFuncs.ClientPrintAll(HUD_PRINTTALK, message);
							}
							//ReAssing new info
							data["IpInfo"] = info;
						}
				}
			}
		}
	});
}
HookReturnCode ClientConnected(edict_t@ eEdict, const string &in sNick, const string &in sIp, bool &out bNoJoin, string &out sReason)
{
	QueryUserCountry(@eEdict, sNick, sIp);
	return HOOK_CONTINUE;
}
