void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE Test3 Plugin\r\n");
	SE.Printf("\r\nSE version: %s\n", SE.VERSION);

	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );
	VoiceOption();
	QueryCvarEvent();
	QueryCvarTrigger();
}

void VoiceOption()
{
	//Admins only voice chat
	bool voiceAdminOnly = SE.Voice_AdminOnly;
	//Voicechat disabled
	bool voiceDisabled = SE.Voice_Disabled;
	//Voicechat mininum level requirements
	int voiceMinLevel = SE.Voice_MinLevel;
	SE.Printf("Voice Disabled: %d\nVoice AdminOnly: %d\nVoiceMinLevel: %d\n", voiceAdminOnly, voiceDisabled, voiceMinLevel);
	
	//Get voice stats for player 1, you can change properties.
	SEVoiceState@ voiceState = SE.GetVoiceState(1);
	if(voiceState !is null)
	{
		//Only players with the same TeamId can hear each other.
		//voiceState.TeamId = 0;
		
		//If the Level value is greater than SE.Voice_MinLevel, other players can hear them.
		//voiceState.Level = 0;
		
		//This is automaticly filled by admins.txt but you can change them
		//If SE.Voice_AdminOnly is set to true, voice chat can only be used by players whose IsAdmin value is true.
		//voiceState.IsAdmin = false;
		
		//Players with Muted set to true cannot be heard by other players.
		//voiceState.Muted = false;
		SE.Printf("Player #1  Voice\nAdmin: %d\nTeam:%d\nMuted:%d\nLevel:%d\n", voiceState.IsAdmin, voiceState.TeamId, voiceState.Muted, voiceState.Level);
	}
}
void QueryCvarEvent()
{
	bool isRegCvar = SE_EVENT.OnGame("playerquerycvar", @ClientQueryCvarValue);
	bool isRegCvar2 = SE_EVENT.OnGame("playerquerycvar2", @ClientQueryCvarValue2);
	SE.Printf("playerquerycvar registered: %d\nplayerquerycvar2 registered: %d\n", isRegCvar, isRegCvar2);
}
void QueryCvarTrigger()
{
	SE.QueryClientCvarValue(1, "rate");
	SE.QueryClientCvarValue2(1, "rate", 156);
}
void ClientQueryCvarValue(CallbackItem@ item, edict_t@ edict, string&in cvar)
{
	if(item is null)
	{
		SE.Printf("Bad cvar\n");
		return;
	}
	string plName = string(edict.vars.netname);
	//SE.Printf("Client cvar player: %s cvar_value: %s\n",  plName,  cvar);
	g_Scheduler.SetTimeout("TimeoutTestForSEEvent", 1.0, plName, cvar);
}
void TimeoutTestForSEEvent(string  plName, string cvar)
{
	SE.Printf("TimoutMsg: Client cvar player: %s cvar_value: %s\n",  plName,  cvar);
}
void ClientQueryCvarValue2(CallbackItem@ item, edict_t@ edict, int requestId, string&in cvarname, string&in value)
{
	if(item is null)
	{
		SE.Printf("Bad cvar2 requestId: %d\n", requestId);
		return;
	}
	string plName = string(edict.vars.netname);
	SE.Printf("Client cvar2 player: %s\ncvar: %s\nrequestId: %d\nvalue: %s\n", plName, cvarname, requestId, value);
}