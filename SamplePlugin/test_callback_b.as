void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE Callback B Plugin\r\n");
	g_EngineFuncs.ServerPrint("\r\nSE version: " + SE.VERSION + "\r\n");
	
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );

	SE.OnReady({"PluginA"}, @ModuleReady);
	//g_EngineFuncs.ServerPrint("fnc retcode: " + string(call2.returnCode) + " \r\n");

}

void ModuleReady()
{
	g_EngineFuncs.ServerPrint("\r\n Callback B Plugin: PluginA is ready\r\n");
	dictionary@ d = {{"map", "svencoop1"}};
	CallbackItem@ callResult = SE.CallMethod("PluginA.ChangeMap", @any(1), @d);
	if(@callResult !is null)
	{
		g_EngineFuncs.ServerPrint(SE.Interpolate("\r\n Result Code: {returnCode}, Result String: {returnString}\r\n", {{"returnCode", callResult.ReturnCode}, {"returnString", callResult.ReturnString}}));
	}
}
void PluginExit()
{
	SE.PluginExit();
}