# OnGame Event Supports
Sven Enhancer supports some OnGame events,

Added SEMessageContext class for message event,
```angelscript
int MsgType;
int MsgDest;
Vector Origin;
//Set true to block message
bool Blocked;
//Target
edict_t@ Edict

//clean=true cleanning \x02 and \n,
string& GetArgString(int index, bool clean=true) const
//for say text
array<string>@ SplitArgString(int index, bool clean=true) const
int GetArgCount() const
int GetArgType(int index) const
int8 GetArgByte(int index) const
int8 GetArgChar(int index) const
int16 GetArgShort(int index) const
int GetArgLong(int index) const
float GetArgFloat(int index) const
edict_t@ GetArgEntity(int index) const
bool IsArgModified(int index) const
void SetArgByte(int index, int8 value)
void SetArgChar(int index, int8 value)
void SetArgShort(int index, int16 value)
void SetArgLong(int index, int value)
void SetArgFloat(int index, float value)
void SetArgEntity(int index, edict_t@ value)
void SetArgEntity(int index, int value)
void SetArgString(int index, string&in value, bool clean = true)
int AddArg(SEMessageArgType type)
//reading 4 byte ofset
float ReadFloat(int index) const
//write 4 byte offset
void SetFloat(int index, float value)
void Clear()
```

Added **SEKeyValueData** class for keyvalue event
```angelscript
string& get_ClassName()
void set_ClassName(const string&in)
string& get_KeyName()
void set_KeyName(const string&in)
string& get_KeyValue()
void set_KeyValue(const string&in)
```

Currently supported events;
```angelscript
  //message.{messageid}
  //Signature for message
  void HookScoreTest(CallbackItem@ item)
  {
    //Prevent to send
    //item.Blocked = true;
    //Reading score
    float score = SE_EVENT.MessageContext.ReadFloat(1);
    //Spoofing score
	  SE_EVENT.MessageContext.SetFloat(1, score + 100);

    //Get admin status
    int8 adminStatus = SE_EVENT.MessageContext.GetArgByte(16);

    //Set admin status to owner
    SE_EVENT.MessageContext.SetArgByte(16, 2);
  }
  bool isRegistered = false;
  //Register score
  int scoreId = 83;
  isRegistered = SE_EVENT.OnGame(SE.Sprintf("message.%d", scoreId), @HookScoreTest);

  //For unregister
  //SE_EVENT.OffGame(SE.Sprintf("message.%d", scoreId));

  //getgamedescription
  void HookGetGameDesc(CallbackItem@ item)
  {
    //Set ReturnCode to change gamedescription
  	item.ReturnCode = 1;
  	item.ReturnString = "SvenEnhacer v1.01";
  }
  isRegistered = SE_EVENT.OnGame("getgamedescription", @HookGetGameDesc);

  //clientuserinfochanged
  void HookUserInfoChanged(CallbackItem@ item, edict_t@ edict, string&in oldInfo, string&in newInfo)
  {
	  //SE.Printf("Older: %s\nNewer: %s\n", oldInfo, newInfo);
	  SE.Printf("Name: %s\n", SE.InfoKeyValue(newInfo, "name"));
    //Set ReturnCode 1 to prevent change
    //item.ReturnCode == 1
  }

  isRegistered = SE_EVENT.OnGame("clientuserinfochanged", @HookUserInfoChanged);

  //keyvalue
  void HookKeyValue(CallbackItem@ item, edict_t@ edict, SEKeyValueData@ keyvalue)
  {
    	//keyvalue.Handled = true;
    	//SE.Printf("Hooked: %s\r\n", keyvalue.ClassName);
      //Set ReturnCode 1 to prevent
      //item.ReturnCode == 1
      //reading keyvalue,
      string key = item.KeyValue;
  }
  isRegistered = SE_EVENT.OnGame("keyvalue", @HookKeyValue);
```


# TOML Support

Adding **TValue** class for **TOML** support

```angelscript
enum TvalueType
{
TVALUE_TYPE_UNDEFINED,
TVALUE_TYPE_OBJECT,
TVALUE_TYPE_ARRAY,
TVALUE_TYPE_STRING,
TVALUE_TYPE_NUMBER,
TVALUE_TYPE_BOOLEAN
}


string& GetString(string& in name = "", bool isPath = false) const
int GetInt(string& in name = "", bool isPath = false) const
int64 GetInt64(string& in name = "", bool isPath = false) const
float GetFloat(string& in name = "", bool isPath = false) const
double GetDouble(string& in name = "", bool isPath = false) const
bool GetBool(string& in name = "", bool isPath = false) const
void SetBool(string& in name, bool value) const
void SetInt(string& in name, int value) const
void SetInt64(string& in name, int64 value) const
void SetFloat(string& in name, float value) const
void SetDouble(string& in name, double value) const
void SetString(string& in name, string& in value) const
bool IsArray() const
TvalueType GetType() const
string& ToString() const
void SaveToFile(const string& in filename) const
void Clear() const
TValue@ At(uint index) const
uint GetSize() const
TValue@ Get(string& in name, bool isPath = false) const
```


Added following methods to **SE** for parsing **TOML** file
```angelscript
  TValue@ TomlParse(string& in input);
  TValue@ TomlParseFromFile(string& in path)
```



**Sample usage**

```angelscript
  string pathToFile = "scripts/plugins/store/tomltest.toml";
  TValue@ toml = SE.TomlParseFromFile(pathToFile);
  //for reading path
  string serverName = toml.GetString("server.name", true);

  //for reading array path
  string mapName = toml.GetString("server.maps[0]", true);

  int numeric = toml.GetInt("server.version", true);

  //Get sub path
  TValue@ subPath = toml.Get("server", true);
```

