void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );
	
	SE_EVENT.On("command.executed#testevent", function (name, callback){
		//Stop calling other command.executed events
		callback.StopCall = true;
		callback.ReturnCode = 1234;
		g_EngineFuncs.ServerPrint(name + " Event Triggered From " + callback.ModuleName + "\r\n");
		//This is remove only $testevent tagget command.executed events
		SE_EVENT.Off("command.executed#testevent");
		//This is remove all  command.executed
		//SE_EVENT.Off("command.executed");
	});

}  