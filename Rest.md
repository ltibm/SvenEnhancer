# SvenEnhancer Rest Support
With SvenEnhancer, you will now be able to make REST requests.

The RestClient supports a synchronous **Execute** method; however, since it will block the server during execution, we recommend using **ExecuteAsync** instead.

# RestUtils
```angelscript
  string hmacsha1 = SE.HMAC_SHA1("key", "data");
  string hmacsha256 = SE.HMAC_SHA256("key", "data");
  string md5 = SE.MD5("data");
```

# RestClient Features
```angelscript
  //Added callback definiton for ExecuteAsync
  void RestCallback(RestContext@ context);

  //Test method for callback
  void AsyncCallback(RestContext@ context){
  }

  uint ConnectionTimeoutMs; //Default 2000
  uint TimeoutMs; //Default 5000
  //Make Request sync, may freeze server until request completed, use ExecuteAsync instead
  RestResponse@ Execute(RestRequest@ request);

  //Make Request async, once it is completed, the callback will be invoked(callback is optional).
  bool ExecuteAsync(RestRequest@ request, RestCallback@ callback = null)
```


# RestRequest Features
```angelscript
  string Url;
  string& get_Body();
  void AddBody(string&in body, string&in contentType ="text/plain");
  void AddJsonBody(?&in obj);
  string Method; //GET, POST, PUT, PATCH, DELETE
  RestRequest@ AddHeader(string&in key, string&in value); //Returning own reference
  RestRequest@ AddQueryParam(string&in key, string&in value) //Returning own reference
  string& GetQueryString();
  void ClearQueryParams();
  void ClearHeaders();
  any@ UserData; //For RestClient.ExecuteAsync
```

# RestResponse Features
```angelscript
  string Body;
  int StatusCode;
  string& GetHeader(string&in key);
  string& GetHeader(uint index);
  string& GetHeaderKey(uint index);
  uint HeaderCount();
  //Parse Body to Json
  JValue@ GetJsonBody();
```

# RestContext Features(For Async Result)
```
  RestContext@ context;
  RestRequest@ Request;
  RestResponse@ Response;
  any@ get_UserData() const;
```

# RestClient Samples
```angelscript
  //For creating client
  RestClient@ client = RestClient();
  //Connection Timeout(default 2000)
  client.ConnectionTimeoutMs = 3000;
  //Timeout(default 5000)
  client.TimeoutMs = 3000;

  //For preparing request
  RestRequest@ request = RestRequest();
  //GET, POST, PUT, PATCH, DELETE
  request.Method = "POST";
  //Set Request Url
	request.Url = "https://www.svencoop.com/api/test";

  //Add Query Paramater
  request.AddQueryParam("param1", "value1");
  request.AddQueryParam("param2", "value2");

  //Add Header Variable
  request.AddHeader("MyHeader", "MyValue");
  request.AddHeader("MyHeader2", "MyValue2");

  //Add Body
  //Content Type default is text/plain
  request.AddBody("Test");
  request.AddBody("Test", "text/plain");

  //You can add json body directly from any object, this is automatcly set Content-Type to application/json
  Dictionary@ dict = {{"key", 1}, {"key2", "value2"}};
  request.AddJsonBody(@dict);

  //Or you can set
  request.AddJsonBody(dictionary = {{"user", "admin"}, {"password", "root"}});

  //if you want to pass custom data;
  @request.UserData = any(2534);

  //For get url query string
  string queryString = request.GetQueryString();


  //Execute Sync(NOT RECOMMENEND this is may freeze your server until request completed);
  RestResponse@ response = client.Execute(@request);

  //Get status code
  int statusCode = response.StatusCode;

  //Get header
  string myHeader = response.GetHeader("MyHeader");

  //Get Header By Index
  myHeader = response.GetHeader(0);

  //Get Header key
  string headerKey = response.GetHeaderKey(0);

  //Get Headers Count
  uint headerCount = response.HeaderCount(0);

  //Get Body
  string body = response.Body;


  //Get Body as Json(this is automatcly parsed)
  JValue@ json = response.GetJsonBody();
  string errorMessage = json.Path("/error").GetString();
  
  

  //Execute Async(RECOMMENEND)
  bool isExecuted = client.ExecuteAsync(@request, @AsyncCallback);

  //Or you can use anonymous function
  client.ExecuteAsync(@request, function(context){
		  //AsyncCallback(@context);
       RestResponse@ response = context.Response;
	});
  
  void AsyncCallback(RestContext@ context){
      RestResponse@ response = context.Response;
      //context.UserData.retrieve(myobj);
  }
  

```


# Rest For Angelscript
[Sample Plugin](https://github.com/ltibm/SvenEnhancer/blob/master/SamplePlugin/test_rest.as)
