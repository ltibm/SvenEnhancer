void Sanane(){
	g_EngineFuncs.ServerPrint("Samanye\r\n");    
}

class AClass{
	string Name;
}


void PluginInit()
{
	g_EngineFuncs.ServerPrint("\r\nSE Test Plugin\r\n");
	g_EngineFuncs.ServerPrint("\r\nSE version: " + SE.VERSION + "\r\n");

	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "test" );
	GlobalsTest();
	InterpolateTest();
	JsonTest();
	JsonSerializeTest();
	JsonDeserializeTest();
	JsonDeserializeTestArray();
	JsonDictTest();
	JsonWithConfig();
	JsonFileTest();
}
void GlobalsTest()
{
	AClass@ aclass = AClass();
	aclass.Name = "Test";
	SE.globals["Test"] = 1;
	SE.globals["Testa"] = 2;
	SE.globals["Testb"] = 3;
	SE.globals["Testc"] = 4;
	SE.globals["Testc"] = 4;
	int numo = 0;
	SE.globals.get("Testf", numo);
	SE.globals["Testf"] = 4 + numo;
	SE.globals["Testd"] = @aclass;
	SE.globals["Teste"] = @aclass;
	int num = 0;
	SE.globals.get("Testf", num);
	
	g_EngineFuncs.ServerPrint("Test assigned to globals\r\n");
	g_EngineFuncs.ServerPrint("TN: " + num + "\r\n");
	
}
void InterpolateTest()
{
	string r2 = SE.Interpolate("Number: {Param1:.2f}, Text: {Param2}", {{"Param1", 1903.01}, {"Param2", "string"}});
	g_EngineFuncs.ServerPrint(r2);
	g_EngineFuncs.ServerPrint("\r\n");
}
string sampleJson = "{\"name\":\"John\", \"age\":30, \"bool\":true}";

void JsonTest()
{
	JValue@ json = SE.JsonParse(sampleJson);
	JValue@ namePath = json.Path("/name");
	JValue@ agePath = json.Path("/age");

	string nameStr = namePath.GetString();
	string jsonStr = json.ToString();
	int ageNum = agePath.GetInt();
	g_EngineFuncs.ServerPrint("\r\n JsonContent \r\n");
	g_EngineFuncs.ServerPrint(SE.Interpolate("Name: {name}, Age: {age}\r\n", {{"name", nameStr}, {"age", ageNum}}));
	g_EngineFuncs.ServerPrint(jsonStr);
	g_EngineFuncs.ServerPrint("\r\n");
	
}

class Member{
	int id = 0;
	string name;
	int age;
	array<string> skills;
	float rating;
	DateTime date;
}
class Member2{
	int age;
	string name;
}

void JsonSerializeTest()
{
	g_EngineFuncs.ServerPrint("\r\nJsonSerializeTest\r\n");
	
	Member@ member = Member();
	member.name = "Jhon";
	member.age = 30;
	member.rating = 3366.30;
	member.skills.insertLast("gaming");
	member.skills.insertLast("driving");
	member.skills.insertLast("coding");
	
	JValue@ serialized = SE.JsonParseFromObject(member);
	g_EngineFuncs.ServerPrint("\r\n Serialized - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(serialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
	
}
void JsonDeserializeTest()
{
	g_EngineFuncs.ServerPrint("\r\nJsonDeserializeTest\r\n");
	string jsonBody = "{\"age\":30,\"name\":\"Jan\",\"skills\":[\"music\",\"book\",\"swim\"]}";
	JValue@ json = SE.JsonParse(jsonBody);
	Member@ member = Member();
	dictionary t;
		member.rating = 255; //null access exception
		
	Member mem2;
	json.Deserialize(@member);

	JValue@ serialized = SE.JsonParseFromObject(member);
	g_EngineFuncs.ServerPrint("\r\n Deserialized - JsonContentf \r\n");
	g_EngineFuncs.ServerPrint(serialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
}
void JsonDeserializeTestArray()
{
	string jsonBody = "[{\"age\":15,\"name\":\"Can\",\"skills\":[\"music\",\"book\",\"swim\"]},{\"age\":20,\"name\":\"Ran\",\"skills\":[\"t\",\"e\",\"f\"]},{\"age\":25,\"name\":\"Ran\",\"skills\":[\"b\",\"j\",\"k\"]}]";
	g_EngineFuncs.ServerPrint("\r\nJsonDeserializeTest Array\r\n");
	JValue@ json = SE.JsonParse(jsonBody);
	array<Member@> members;
	json.Deserialize(@members);
	JValue@ serialized = SE.JsonParseFromObject(members);
	g_EngineFuncs.ServerPrint("\r\n Deserialized Array - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(serialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");

}
void JsonDictTest()
{
	dictionary@ t = {
		{"name", "Jhon"},
		{"age", 30},
		{
			"vars",
			dictionary  = {{"var", "value1"}}
		}
	};
	JValue@ serialized = SE.JsonParseFromObject(t);
	g_EngineFuncs.ServerPrint("\r\n Deserialized Dictionary - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(serialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
	
	string jsonBody = "{\"age\":30,\"name\":\"Jan\",\"skills\":[\"music\",\"book\",\"swim\"], \"vars\": {\"a\": 1}}";
	//string jsonBody = "{\"age\":30,\"name\":\"Jan\", \"vars\": {\"a\": 1}, \"vars2\": {\"b\": 1}}";
	//string jsonBody = "{\"skills\":[\"music\",\"book\",\"swim\"]}";
	//jsonBody = "{\"skills\":[1,2,3,4,5]}";
	dictionary@ t2 = dictionary();
	JValue@ json = SE.JsonParse(jsonBody);
	json.Deserialize(@t2);
	JValue@ deserialized = SE.JsonParseFromObject(t2);
	g_EngineFuncs.ServerPrint("\r\n Deserialized Dictionary - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(deserialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
	
}
class SubClass{
	string subName;
	void __JSConfig(JSerializeConfig@ config)
	{
		config
		.PropName("subName", "SubName");
	}
}
class Member3
{
	string m_Age;
	SubClass@ subClass;
    string Age 
    { 
      get const 
      { 
        return m_Age; 
      } 
      set 
      { 
        m_Age = value; 
      }
    }
	void __JSConfig(JSerializeConfig@ config)
	{
		config
		.PropName("subClass", "SubClass", JOPT_READWRITE)
		.Ignore("m_Age");
	}
}

void JsonWithConfig()
{
	Member3 member;
	@member.subClass = SubClass();
	member.subClass.subName = "SC";
	JValue@ serialized = SE.JsonParseFromObject(member);
	g_EngineFuncs.ServerPrint("\r\n Serialized Json WithConfig - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(serialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
	
	string jsonBody = "{\"Age\":\"16\",\"m_Age\":\"4444\",\"SubClass\":{\"SubName\":\"SC\"}}";
	Member3 memberdeserialized;
	JValue@ json = SE.JsonParse(jsonBody);
	json.Deserialize(@memberdeserialized);
	JValue@ deserialized = SE.JsonParseFromObject(memberdeserialized);
	g_EngineFuncs.ServerPrint("\r\n Deserialized Json WithConfig - JsonContent \r\n");
	g_EngineFuncs.ServerPrint(deserialized.ToString());
	g_EngineFuncs.ServerPrint("\r\n");
}

void JsonFileTest()
{
	JValue@ inputJson = SE.JsonParseFromFile("scripts/plugins/store/json_input.json");
	inputJson.Set("age", 999);
	array<string> keys = inputJson.GetKeys();
	inputJson.Set("keys", keys);
	JValue@ myPath = inputJson.Path("/path");
	myPath.Set("sub", 266);
	inputJson.Set("path", @myPath);
	inputJson.SaveToFile("scripts/plugins/store/json_output.json", 4);
	g_EngineFuncs.ServerPrint("\r\n Json Read from file and Save to file completed \r\n");
}