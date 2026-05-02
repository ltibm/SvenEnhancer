
void PluginInit()
{



	dictionary d;
	d["name"] = "Ahmet";
	d["sk"] = 1336;
	dictionary dc = {{
		"sv", "subvalue"
	}};
	d["subdict"] = dc;
	int sk = d.GetInt("sk");
	SE.Printf("v: %d\n", sk);
	dictionary@ subDict = d.GetDict("subdict");
	if(subDict is null)
		SE.Printf("sv is null\n");
	else{
		string ss = subDict.GetString("sv");
		SE.Printf("v: %s\n", ss);
	}
	TomlTest();
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );
	g_EngineFuncs.ServerPrint("Caller loaded\r\n");
	bool isreg = false;
	isreg = SE_EVENT.OnGame("message.74", @HookSayText);
	//isreg = SE_EVENT.OnGame("message.83", @HookScoreTest);
	isreg = SE_EVENT.OnGame("getgamedescription", @HookGetGameDesc);
	//isreg = SE_EVENT.OnGame("clientuserinfochanged", @HookUserInfoChanged);
	//isreg = SE_EVENT.OnGame("keyvalue", @HookKeyValue);
	SE.Printf("Hooked edildi: %d\r\n", isreg);
}
void TomlTest()
{
	string pathToFile = "scripts/plugins/store/tomltest.toml";
	TValue@ toml = SE.TomlParseFromFile(pathToFile);
	if(toml !is null)
	{
		string sname = toml.GetString("server.name", true);
		TValue@ inner = toml.Get("server.roles", true);
		if(inner !is null)
		{
			string ival =  inner.GetString("[0].steamid", true);

			SE.Printf("size: %d, v: %s\n", inner.GetSize(), ival);
		}
		SE.Printf("v: %s\n", sname);
		
	}
}
void HookKeyValue(CallbackItem@ item, edict_t@ edict, SEKeyValueData@ keyvalue)
{
	//keyvalue.Handled = true;
	//SE.Printf("Hooked: %s\r\n", keyvalue.ClassName);
}
void HookUserInfoChanged(CallbackItem@ item, edict_t@ edict, string&in oldInfo, string&in newInfo)
{
	//SE.Printf("Eskisi: %s\nYenisi: %s\n", oldInfo, newInfo);
	SE.Printf("Name: %s\n", SE.InfoKeyValue(newInfo, "name"));
}
void HookGetGameDesc(CallbackItem@ item)
{
	item.ReturnCode = 1;
	item.ReturnString = "SvenEnhacer v1.0";
}
void HookSayText(CallbackItem@ item)
{
	int plId = SE_EVENT.MessageContext.GetArgByte(0);

	string msg = SE_EVENT.MessageContext.GetArgString(1);
	

	if(plId > 0)
	{
		CBasePlayer@ plr = g_PlayerFuncs.FindPlayerByIndex(plId);
		if(plr is null)
			return;
		string name = string(plr.pev.netname);
		array<string> splitted = SE_EVENT.MessageContext.SplitArgString(1);
		SE.Printf("length: %d\n", splitted.length());
		if(splitted.length() > 1)
		{
			if(splitted[0] == name)
			{
				SE_EVENT.MessageContext.SetArgString(1, SE.Sprintf("[Test]%s: %s", splitted[0], splitted[1]));
			}

		}

	}
	
	//SE_EVENT.MessageContext.SetArgString(1, "Gamer S!: Sanane hıyarto");

}
void HoomTeamInfo(CallbackItem@ item)
{
}
void HookScoreTest(CallbackItem@ item)
{
	//spoof score
	float score = SE_EVENT.MessageContext.ReadFloat(1);
	SE_EVENT.MessageContext.SetFloat(1, score + 100);
	//spoof deaths
	SE_EVENT.MessageContext.SetArgLong(5, 20);
	//health and armor
	float health = SE_EVENT.MessageContext.ReadFloat(6);
	float armor = SE_EVENT.MessageContext.ReadFloat(10);
	
	
	//Spoof team
	SE_EVENT.MessageContext.SetArgByte(14, 17);
	//Donor status
	SE_EVENT.MessageContext.SetArgByte(15, 1);
	
	//Admin status
	SE_EVENT.MessageContext.SetArgByte(16, 0);
	
	
	SE.Printf("Armor: %f \r\n", armor);
	//SE.Printf("ArgCount: %d", SE_EVENT.MessageContext.GetArgCount());
	//int argindex = 	SE_EVENT.MessageContext.AddArg(SEMSG_BYTE);
	//SE_EVENT.MessageContext.SetArgByte(argindex, 5);
	//context.MsgType;
	//context.MsgDest;
	//context.SetArgByte(3, 5);
	//context.ArgC();
	//context.SetArgString(1, "Lan yürü git");
}
int GenerateNumber(int minlen = 4, int maxlen = 4)
{
	if(maxlen <= 0) return 0;
	if(minlen < 1) minlen = 1;
	if(minlen > maxlen) minlen = maxlen;
	int mlen = maxlen;
	if(minlen != maxlen)
	{
		mlen = Math.RandomLong(minlen, maxlen);
	}
	string generated = "";
	for(int i = 0; i < mlen; i++)
	{
		if(i == 0)
		{		
			generated += string(Math.RandomLong(1, 9));
		}
		else
		{	
			generated += string(Math.RandomLong(0, 9));
		}
	}
	return atoi(generated);
}