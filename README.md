# Sven Enhancer
Sven Enhancer is a Metamod plugin for SvenCoop, this plugin is tested on Windows 11 and Ubuntu(With WSL).

# Entity Support
[SEE](https://github.com/ltibm/SvenEnhancer/blob/master/Entity.md)

# REST(WebRequest) Support
[SEE](https://github.com/ltibm/SvenEnhancer/blob/master/Rest.md)

# MySql Support
[SEE](https://github.com/ltibm/SvenEnhancer/blob/master/MySql.md)

# Requirements
https://github.com/hzqst/metamod-fallguys

**For Linux**: asext.so

**For Windows**: asext.dll

# Installing
Shutdown server
If metamod-fallGuys is not installed, download and install it, then copy it into the **addons** folder.
Download latest version of **SvenEnhancer** and extrat to **svencoop** folder,
open plugins.ini from **svencoop/addons/metamod** and put if not exists;
```
win32 addons/metamod/dlls/SvenEnhancer.dll

linux addons/metamod/dlls/SvenEnhancer.so
```
save and start server again.


# Features
SvenEnhancer provides some AngelScript enhancements for SvenCoop. The current features are listed below:

- JSON support for the game has been added using the nlohmann::json library.
- And most importantly, serialize and deserialize support has been added in a way similar to the .NET JSON library, you can convert your classes directly to JSON or populate your class from JSON data.
- Added Interpolate functions for interpolating strings
- A globals variable has been added, which can be used by all plugins
- For more details, you can check the SamplePlugin folder

# Globals
 ```angelscript
    //Global property
    //SvenEnhancer@ SE

    //you can check version
    //string& get_VERSION() const
    string version = SE.VERSION;

    //string& Interpolate(string& in input, dictionary@ params)
    //excepted: Number: 1903.01, Text: string
    string interpolate = SE.Interpolate("Number: {Param1:.2f}, Text: {Param2}", {{"Param1", 1903.01}, {"Param2", "string"}}):

    //JValue@ JsonParse(string& in input)
    JValue@ simpleJson = SE.JsonParse("{\"Name\": "\"Ibm\"}");

    //json content
    string jsonResult = simpleJson.ToString();

    //parse from dictionary, class and array supported
    //JValue@ JsonParseFromObject(?&in obj)
  	dictionary@ t = {
  		{"name", "Jhon"},
  		{"age", 30},
  	};
    JValue@ serialized = SE.JsonParseFromObject(t);
    string serializedJson = serialized.ToString();
    
    //parse from file
    //JValue@ JsonParseFromFile(string& in path)
    JValue@ serialized = SE.JsonParseFromFile("path_to_.json");


    //const int JOPT_READ
    //const int JOPT_WRITE
    //const int JOPT_READWRITE
    //For serializing and deserializing options.
  ```

# Examples
```angelscript
  //For interpolation
  string interpolate = SE.Interpolate("Number: {Param1:.2f}, Text: {Param2}", {{"Param1", 1903.01}, {"Param2", "string"}}):


  //For serializing class
  class MyTestClass
  {
    	int age;
  	  string name;
	    array<string> testStringList;
  }

    MyTestClass@ myTestClass = MyTestClass(); //Or MyTestClass myTestClass;
    myTestClass.age = 99;
    myTestClass.name = "Jhon Does";
  	myTestClass.testStringList.insertLast("t1");
  	myTestClass.testStringList.insertLast("t2");
  	myTestClass.testStringList.insertLast("t3");

    JValue@ serializedmyTestClass = SE.JsonParseFromObject(member);
    //Serialized json result
    string myTestJsonResult = serializedmyTestClass.ToString();
    //For deserializing class
    MyTestClass myTestClass2; //Or MyTestClass@ myTestClass2 = MyTestClass();
    //You must use handler for true result, otherwise result may empty or default
    //All json variables passed to myTestClass2
    serializedmyTestClass.Deserialize(@myTestClass2);

    //Some json options
    string jsonBody = "{\"age\":30,\"name\":\"Jan\"}";
  	JValue@ json = SE.JsonParse(jsonBody);
    //Use json path
    JValue@ age = json.Path("/age");
    //or GetBool(), GetDouble()
    int age = age.GetInt();

    //Check json key
    bool existsKey = json.ContainsKey("skills");

  //for clear all member
    json.Clear();


    //Json Config support for serializing and deserializing classes

    //This plugin use JSerializeConfig class
    //JSerializeConfig@ Ignore(string& in name, int flags = JOPT_READWRITE) const
    //JSerializeConfig@ PropName(string& in name, string& in _alias, int flags = JOPT_READWRITE) const
    //JSerializeConfig@ Clear() const

    //Before performing serialize/deserialize operations, if defined in your class, the following method will always be called:
    //void __JSConfig(JSerializeConfig@ config)

    //for Example

    class MyClass2
    {
        int _m_points;
        string Rank;
        sting name;
        int Points 
        { 
          get const 
          { 
            return _m_points; 
          } 
          set 
          { 
            _m_points = value; 
          }
        }
      	void __JSConfig(JSerializeConfig@ config)
      	{
          //DEFAULTS JOPT_READWRITE
      		config
      		.Ignore("_m_points") //_mpoints property is ignored 
          .Ignore("Rank", JOPT_READ) //Rank property can serialize but cant deserialize
          .PropName("name", "Name");
      	}
    }

```


# Json Serializing / Deserializing Supports
- int
- double
- bool
- DateTime (with unixtimestamp)
- dictionary
- array
- string
