TValue@ userTags = null;
TValue@ tagsDisabled = null;
TValue@ colorsDisabled = null;
string userChatFormat = "%s %s: %s";
bool isLoaded = false;
string pathToCfg = "scripts/plugins/config/se_usertags.toml";
bool featureEnabled = false;
Cvar@ cvar_Enabled;
bool colorDisabledForThisMap = false;
bool tagDisabledForThisMap = false;
array<int> userLastDonorStatus(33);
void PluginInit()
{
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "steam:ibmlt" );
	g_Hooks.RegisterHook(Hooks::Player::ClientPutInServer, @ClientPutInServer);
	@cvar_Enabled = SE.RegisterCvar("se_tags_enabled", "1", FCVAR_NONE, 1.0f);
	SE.RegisterClientCmd("se_reloadtags", @ReloadUserTagsCmd, 999);
	SE.RegisterServerCmd("se_reloadtags", @ReloadUserTagsServerCmd, 999);
	//Hooking for score and saytext
	SE_EVENT.OnGame("message.74", @HookSayText);
	SE_EVENT.OnGame("message.83", @HookScore);
	LoadTags();
}
void MapInit()
{
	CheckMapDisable(g_Engine.mapname);
}
void CheckMapDisable(string mapName)
{
	if(!isLoaded)
		return;
	tagDisabledForThisMap = false;
	colorDisabledForThisMap = false;
	if(tagsDisabled !is null)
	{
		uint size = tagsDisabled.GetSize();
		for(uint i = 0; i < size; i++)
		{
			TValue@ item = tagsDisabled.At(i);
			if(item.GetString() == mapName)
			{
				tagDisabledForThisMap = true;
				break;
			}
		}
	}
	if(colorsDisabled !is null)
	{
		uint size = colorsDisabled.GetSize();
		for(uint i = 0; i < size; i++)
		{
			TValue@ item = colorsDisabled.At(i);
			if(item.GetString() == mapName)
			{
				colorDisabledForThisMap = true;
				break;
			}
		}
	}
}
HookReturnCode ClientPutInServer(CBasePlayer@ pPlayer)
{
    if (pPlayer is null)
        return HOOK_CONTINUE;
	userLastDonorStatus[pPlayer.entindex()] = 0;
	LoadUserTag(@pPlayer);
    return HOOK_CONTINUE;
}
void LoadTags()
{
	isLoaded = false;
	TValue@ toml = SE.TomlParseFromFile(pathToCfg);

	if(toml is null)
		return;

	@userTags = toml.Get("usertags");
	featureEnabled = true;// toml.GetBool("enabled");
	isLoaded = userTags !is null;
	
	LoadAllUserTags();
	userChatFormat = toml.GetString("chatformat", true);
	@tagsDisabled = toml.Get("tags_disabled_maps");
	@colorsDisabled = toml.Get("color_disabled_maps");
	CheckMapDisable(g_Engine.mapname);
}
void LoadAllUserTags()
{
	if(!isLoaded) return;

	for (int i = 1; i <= g_Engine.maxClients; i++)
    {
        CBasePlayer@ pPlayer = g_PlayerFuncs.FindPlayerByIndex(i);
		if(pPlayer is null || !pPlayer.IsConnected())
			continue;
		LoadUserTag(@pPlayer);
	}
}
void LoadUserTag(CBasePlayer@ player)
{	
	if(!isLoaded || player is null)
		return;

	string steamid = g_EngineFuncs.GetPlayerAuthId(player.edict());
	if(steamid == "") return;
	dictionary@ data = player.GetData();
	if(data is null)
		return;
	TValue@ t_UserData = FindUserTag(steamid);
	if(t_UserData !is null)
	{
		bool disabled =  t_UserData.GetBool("disabled");
		if(disabled)
		{
			if(data.exists("se_tag"))
				data.delete("se_tag");
			return;
		}
		dictionary tagData;
		string tag = t_UserData.GetString("tag");
		string color = t_UserData.GetString("color");
		bool usechatformat = t_UserData.GetBool("nochatformat") == false;

		int colorTeam = ColorTeam(color);
		tag.Trim();
		tagData["Tag"] = tag;
		tagData["ColorTeam"] = colorTeam;
		tagData["UseChatFormat"] = usechatformat;
		data["se_tag"] = tagData;
	}
	else{
		if(data.exists("se_tag"))
			data.delete("se_tag");
	}

}
TValue@ FindUserTag(string steamid)
{
	uint size = userTags.GetSize();
	for(uint i = 0; i < size; i++)
	{
		TValue@ item = userTags.At(i);
		if(item.GetString("steamid") == steamid)
			return item;
	}
	return null;
}

int ColorTeam(string color)
{
	if(color == "Blue")
		return 16;
	else if(color == "Red")
		return 17;
	else if(color == "Yellow")
		return 18;
	else if(color == "Green")
		return 19;
	return -1;
}
void ReloadUserTagsServerCmd(CallbackItem@ callback)
{
	LoadTags();
	SE.Printf("All tags reloaded\n");
}
int ReloadUserTagsCmd(edict_t@ edict, CallbackItem@ callback)
{
	callback.StopCall = true;
	CBasePlayer@ pPlayer = g_PlayerFuncs.FindPlayerByIndex(g_EntityFuncs.EntIndex(edict));
    if (pPlayer is null)
        return 1;
	if(g_PlayerFuncs.AdminLevel(@pPlayer) != ADMIN_OWNER)
		return 0;
	LoadTags();
	g_PlayerFuncs.ClientPrint(pPlayer, HUD_PRINTCONSOLE, "All tags reloaded\n");
	return 1;
}

void HookSayText(CallbackItem@ item)
{
	if(tagDisabledForThisMap || cvar_Enabled.value <= 0)
		return;
	//no more set string data if modified
	if(!featureEnabled || SE_EVENT.MessageContext.IsArgModified(1))
		return;
	int plId = SE_EVENT.MessageContext.GetArgByte(0);
	if(plId > 0)
	{
		userLastDonorStatus[plId] = SE_EVENT.MessageContext.GetArgByte(15);
		string msg = SE_EVENT.MessageContext.GetArgString(1);
		CBasePlayer@ plr = g_PlayerFuncs.FindPlayerByIndex(plId);
		if(plr is null)
			return;
		string name = string(plr.pev.netname);
		array<string> splitted = SE_EVENT.MessageContext.SplitArgString(1);
		if(splitted.length() > 1)
		{
			int matchStatus = 0;
			if(splitted[0] == name)
				matchStatus = 1;
			else if(splitted[0] == "(TEAM) " + name)
				matchStatus = 2;
			if(matchStatus > 0)
			{
				string tPrefix = "";
				if(matchStatus == 2)
					tPrefix = "(TEAM) ";
				dictionary@ data = plr.GetData();
				if(data is null || !data.exists("se_tag"))
					return;
				dictionary tagData;
				if(!data.get("se_tag", tagData))
					return;
				bool useFormat = tagData.GetBool("UseChatFormat");
				if(!useFormat)
					return;
				string tag = tagData.GetString("Tag");

				if(tag.Length() > 0)
				{
					SE_EVENT.MessageContext.SetArgString(1, tPrefix + SE.Sprintf(userChatFormat, tag, name, splitted[1]));
				}
			}
		}
	}
}
void SendScoreMessage(CBasePlayer@ player, int newTeam = -1)
{
	int index = player.entindex();
	int adminIdx = g_PlayerFuncs.AdminLevel(@player);
	NetworkMessage msg(MSG_ALL, NetworkMessages::ScoreInfo);
    msg.WriteByte(index);
    msg.WriteFloat(player.pev.frags);
	msg.WriteLong(player.m_iDeaths);
    msg.WriteFloat(player.pev.health);
    msg.WriteFloat(player.pev.armorvalue);
	msg.WriteByte(newTeam == -1 ? player.GetClassification(0) : newTeam);
	msg.WriteByte(userLastDonorStatus[index]);
	msg.WriteByte(adminIdx);
    msg.End();
}

void HookScore(CallbackItem@ item)
{
	if(colorDisabledForThisMap || cvar_Enabled.value <= 0)
		return;
	if(!featureEnabled || SE_EVENT.MessageContext.IsArgModified(14))
		return;
	int plId = SE_EVENT.MessageContext.GetArgByte(0);
	if(plId > 0)
	{
		CBasePlayer@ plr = g_PlayerFuncs.FindPlayerByIndex(plId);
		if(plr is null)
			return;
		dictionary@ data = plr.GetData();
		if(data is null || !data.exists("se_tag"))
			return;
		dictionary tagData;
		if(!data.get("se_tag", tagData))
			return;
		int teamId =  tagData.GetInt("ColorTeam");
		if(teamId != -1)
		{
			SE_EVENT.MessageContext.SetArgByte(14, teamId);
		}
	}
}