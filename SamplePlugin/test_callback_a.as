void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE Callback A Plugin\r\n");
	g_EngineFuncs.ServerPrint("\r\nSE version: " + SE.VERSION + "\r\n");
	
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );
	SE.Register("PluginA");
	//PluginA.ChangeMap
	bool v = SE.RegisterMethod("ChangeMap", @TestChangeMap);
	SE.Ready();	
	g_EngineFuncs.ServerPrint("Callback A Plugin Registered And Triggered\r\n");



}

CallbackItem@ TestChangeMap(string&in name, any@ data = null, dictionary@ dict = null)
{
	string map = "(not passed)";
	if(@dict !is null)
	{
		dict.get("map", map);
	}
	g_EngineFuncs.ServerPrint("\r\nSE Callback A Plugin TestChangeMap is Called: map=" + map + "\r\n");
	CallbackItem@ item = CallbackItem();
	item.ReturnCode = 100;
	item.StopCall = true;
	item.ReturnString = "This is return string";
	return item;
}
void PluginExit()
{
	SE.PluginExit();
}