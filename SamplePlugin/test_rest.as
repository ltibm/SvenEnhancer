void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE Rest Test Plugin\r\n");
	g_EngineFuncs.ServerPrint("\r\nSE version: " + SE.VERSION + "\r\n");
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );
	UtilsTest();
	SyncTest();
	AsyncTest();
	AsyncTest2();
}
void UtilsTest()
{
	string hmacInput = "hinput";
	string hmacKey = "hkey";
	string h256 = SE.HMAC_SHA256(hmacKey, hmacInput);
	string h1 = SE.HMAC_SHA1(hmacKey, hmacInput);
	string md5 = SE.MD5(hmacInput);
	g_EngineFuncs.ServerPrint(SE.Interpolate("\r\nKey: {key}, Data: {data}\r\nHMAC_SHA256: {h256}\r\nHMAC_SHA1: {h1}\r\nMD5: {md5} \r\n", {
		{"data", hmacInput},
		{"key", hmacKey},
		{"h256", h256},
		{"h1", h1},
		{"md5", md5}
	}));

}
void SyncTest()
{
	RestClient@ client = RestClient();
	RestRequest@ request = RestRequest();
	request.Method = "POST";
	request.Url = "https://httpbin.org/post";
	request.AddQueryParam("param1", "value1");
	request.AddHeader("MyHeader", "MyValue");
	request.AddBody("Test", "text/plain");
	//request.AddJsonBody(dictionary = {{"user", "admin"}, {"password", "root"}});
	@request.UserData = any(2534);
		
	g_EngineFuncs.ServerPrint(SE.Interpolate("\r\nQuery String: {queryString} \r\n", {
		{"queryString", request.GetQueryString()}
	}));
	
	RestResponse@ response = client.Execute(@request);
	g_EngineFuncs.ServerPrint("\r\n SYNC HC:" + response.HeaderCount() + "\r\n");
	g_EngineFuncs.ServerPrint("\r\n SYNC HC:" + response.GetHeaderKey(1) + "\r\n");
	g_EngineFuncs.ServerPrint("\r\n SYNC HC:" + response.GetHeader("content-type") + "\r\n");
	g_EngineFuncs.ServerPrint("\r\n SYNC:" + response.Body + "\r\n");
}
void AsyncTest()
{
	RestClient@ client = RestClient();
	RestRequest@ request = RestRequest();
	request.Method = "POST";
	request.Url = "https://httpbin.org/post";
	request.AddQueryParam("param1", "value1");
	request.AddHeader("MyHeader", "MyValue");
	request.AddJsonBody(dictionary = {{"user", "admin"}, {"password", "root"}});
	@request.UserData = any(2534);
	
	client.ExecuteAsync(@request, function(context){
		g_EngineFuncs.ServerPrint("\r\n ASYNC ST:" + context.Response.StatusCode + "\r\n");
		g_EngineFuncs.ServerPrint("\r\n ASYNC:" + context.Response.Body + "\r\n");
		g_EngineFuncs.ServerPrint("\r\n ASYNC HC:" + context.Response.HeaderCount() + "\r\n");
		g_EngineFuncs.ServerPrint("\r\n ASYNC HC:" + context.Response.GetHeaderKey(1) + "\r\n");
		g_EngineFuncs.ServerPrint("\r\n ASYNC HC:" + context.Response.GetHeader("content-type") + "\r\n");
		g_EngineFuncs.ServerPrint("\r\n ASYNC:" + context.Response.Body + "\r\n");
		int myData;
		context.UserData.retrieve(myData);
		g_EngineFuncs.ServerPrint("\r\n ASYNC MYDATA:" + myData + "\r\n");
		//Parse body to json,
		JValue@ json = context.Response.GetJsonBody();
		string mySendedAgs = json.Path("/json").ToString();
		g_EngineFuncs.ServerPrint("\r\n ASYN SENDED ARGS:" + mySendedAgs + "\r\n");


	});
	g_EngineFuncs.ServerPrint("\r\nAsync Request Has Been Send\r\n");

}
void AsyncTest2()
{
	RestClient@ client = RestClient();
	RestRequest@ request = RestRequest();
	request.Method = "GET";
	request.Url = "https://jsonplaceholder.typicode.com/todos/1";
	client.ExecuteAsync(@request, @AsyncCallback);
}

void AsyncCallback(RestContext@ context)
{
	g_EngineFuncs.ServerPrint("\r\n ASYNC2 ST:" + context.Response.StatusCode + "\r\n");
	g_EngineFuncs.ServerPrint("\r\n ASYNC2:" + context.Response.Body + "\r\n");
}