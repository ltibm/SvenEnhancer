SQLiteConnection@ connection = SQLiteConnection();
//based by sqlite
KeyValueStore@ store = KeyValueStore("scripts/plugins/store/mykvstore.db3");
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );
	bool isOpen = false;
	connection.File = "scripts/plugins/store/mydb.db3";
	connection.Open();
	SqliteSyncTest();
	SqliteAsyncTest();
	
	store.Open();
	KeyValueStoreSyncTest();
	KeyValueStoreAsyncTest();

}  

void PrintLN(string v)
{
	g_EngineFuncs.ServerPrint("\r\n" + v + "\r\n");
}

void PluginExit()
{	
	connection.Close();
	store.Close();
}

void KeyValueStoreSyncTest()
{
	string key = "SvenCoop";
	bool exists = store.Exists(key);
	PrintLN("exists: " + string(exists)); 
	store.Set(key, "MyValue");
	store.Set("key2", "v2");
	store.Set("key3", "v2");
	store.Set("key4", "v2");
	string current = store.Get(key);
	PrintLN("current: " + current); 
	store.Delete("key2");
	int total = store.Count();
	PrintLN("total: " + total); 
	store.DeleteAll();
}

void KeyValueStoreAsyncTest()
{
	store.GetAsync("key3", function(result, userdata){
		PrintLN("key: " + result.Key);
		PrintLN("value: " + result.Value);
		PrintLN("exists: " + string(result.Exists)); 
	});
	store.SetAsync("key3", "valuenew", function(result, userdata){
		PrintLN("Success Set: " + result.Key); 
	});
	store.DeleteAsync("key3", function(result, userdata){
		PrintLN("Success Delete: " + result.Key); 
	});
	store.CountAsync(function(result, userdata){
		PrintLN("Count: " + string(result.Count)); 
	});
	store.DeleteAllAsync(function(result, userdata){
		PrintLN("Total Deleted Keys: " + string(result.Count)); 
	});
}

void SqliteSyncTest()
{
	array<SQLiteRow@> rows = connection.Execute("SELECT * FROM PlayerStats WHERE SteamId=@SteamId AND Frags=@Frags", {{"SteamId", "STEAT_12347"}, {"Frags", 28}});
	g_EngineFuncs.ServerPrint("SQLite Status: {rows} \r\n".Fmt({{"rows", string(rows.length())}}));
	for(uint i = 0; i < rows.length(); i++)
	{
		SQLiteRow@ row = rows[i];
		g_EngineFuncs.ServerPrint("{idx}: SteamID: {steam}, Frags: {frags}, Deaths: {deaths}\r\n".Fmt({
			{"idx", i},
			{"steam", row.GetString("SteamID")},
			{"frags", row.GetInt("Frags")},
			{"deaths", row.GetInt("Deaths")}
		}));
	}
}
void SqliteAsyncTest()
{
	connection.ExecuteAsync("SELECT * FROM PlayerStats ", {{"SteamId", "STEAT_12347"}, {"Frags", 28}}, function(rows, userdata){
		g_EngineFuncs.ServerPrint("ASYNC: SQLite Status: {rows} \r\n".Fmt({{"rows", string(rows.length())}}));
		for(uint i = 0; i < rows.length(); i++)
		{
			SQLiteRow@ row = rows[i];
			g_EngineFuncs.ServerPrint("ASYNC: {idx}: SteamID: {steam}, Frags: {frags}, Deaths: {deaths}\r\n".Fmt({
				{"idx", i},
				{"steam", row.GetString("SteamID")},
				{"frags", row.GetInt("Frags")},
				{"deaths", row.GetInt("Deaths")}
			}));
		}
	});
}