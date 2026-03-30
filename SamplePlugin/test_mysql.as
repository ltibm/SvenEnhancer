MySqlConnection@ connection;
void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE MySql Plugin\r\n");
	g_EngineFuncs.ServerPrint("\r\nSE version: " + SE.VERSION + "\r\n");


	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );
	MySqlConnectionConfig config;
	config.Host = "127.0.0.1";
	//No need to set if mysql server using 3306;
	config.Port = 3306;
	config.Username = "root";
	config.Password = "1234";
	config.Database = "svenDB";

	@connection = SE.MySql_CreateConnection(config);
	if(@connection !is null)
	{
		if(connection.Open())
		{
			DropTable();
			CreateTable();
			InsertTest();
			UpdateTest();
			SelectTest();
		}
		else{
			g_EngineFuncs.ServerPrint(SE.Interpolate("\r\nError {error} \r\n", {{"error", connection.GetError()}}));
		}
	}
	else{
		g_EngineFuncs.ServerPrint("\r\nSE Mysql Not Loaded\r\n");

	}
}
void DropTable()
{
	connection.Query("DROP TABLE IF EXISTS testSvenUser");
}
void CreateTable()
{
	connection.Query("CREATE TABLE IF NOT EXISTS testSvenUser (Id INT AUTO_INCREMENT PRIMARY KEY, Name VARCHAR(100), Level INT);");
}
void InsertTest()
{
	if(connection.Query("INSERT INTO testSvenUser (Name, Level) VALUES ('Sven', 10), ('Enhancer', 20), ('SvenEnhancer', 30), ('SvenCoop', 30);"))
	{
		g_EngineFuncs.ServerPrint(SE.Interpolate("\r\n{rows} Inserted\r\n", {{"rows", connection.AffectedRows()}}));
	}
}
void UpdateTest()
{
	connection.Query(SE.Interpolate("UPDATE testSvenUser SET Level = 40 WHERE Name='{name}'", {{"name", connection.Escape("SvenCoop")}}));
}
void SelectTest()
{
	if(connection.Query("SELECT * FROM testSvenUser Order By Level DESC  LIMIT 10"))
	{
		MySqlStoreResult@ results = connection.StoreResult();
		if(@results !is null)
		{
			MySqlRow@ row = results.FetchRow();
			while(@row !is null)
			{
				int level = row.GetInt("Level"); //OR COLUMN Index
				string name = row.GetString("Name");
				int id = row.GetInt("Id");
				g_EngineFuncs.ServerPrint(SE.Interpolate("\r\nId: {id}, Name: {name}, Level :{level}\r\n", {{"id", id}, {"name", name}, {"level", level}}));
				@row = results.FetchRow();
			}
			results.Free();
		}
	}
}
void PluginExit()
{
	if(@connection !is null){
		connection.Close();
	}
}