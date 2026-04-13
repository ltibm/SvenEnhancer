# SQLiteConnection
- SQLite support includes both synchronous and asynchronous queries
- You can assign simple parameters by creating a dictionary object.

```angelscript
//Constructor
SQLiteConnection@ SQLiteConnection();
string& get_File() const;
void set_File(string& in file)
SQLITE_OPEN_FLAG Flags;
bool Open() const;
void Close();
bool IsOpened() const;
string& ErrorMessage();
void SqliteResultCallback(array<SQLiteRow@> rows, any@ userdata = null);
array<SQLiteRow@>@ Execute(const string &in query, dictionary@ params = null);
bool ExecuteAsync(const string &in query, SqliteResultCallback@ callback, any@ userdata = null);
bool ExecuteAsync(const string &in query, dictionary @params, SqliteResultCallback@ callback, any@ userdata = null);
```

# SQLiteRow
```angelscript
string& GetString(string&in column) const;
int GetInt(string&in column) const;
double GetDouble(string&in column) const;
float GetFloat(string&in column) const;
bool GetBool(string&in column) const;
string& GetColumnName(uint index) const;
int ColumnsCount() const;
bool IsNull() const;
SQLITE_TYPE GetColumnType(string&in column) const;
```

# Sample Plugin For SQLite
```angelscript
SQLiteConnection@ connection = SQLiteConnection();
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );
	connection.File = "scripts/plugins/store/mydb.db3";
  //this is default flags
  connection.Flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	bool isOpened = connection.Open();
	SyncTest();
	AsyncTest();
}
void SyncTest()
{
    string query = "SELECT * FROM PlayerStats LIMIT 10";
    array<SQLiteRow@> rows = connection.Execute(query);
    SyncAndAsyncResult(rows, @null);
}
void AsyncTest()
{
    //You can assign simple parameters by creating a dictionary object
    string query = "SELECT * FROM PlayerStats WHERE SteamID=@steamid";

    //Supported parameter types: number, string and datetime(unix)
    dictionary@ queryParams = {
      {"steamid", "STEAM_1234567"},
      {"level", 100}
    };
    //This operation will be added to the query queue and a callback will be triggered upon completion; the game will not freeze during this process.
    bool isQueued = connection.ExecuteAsync(query, @queryParams, @SyncAndAsyncResult);
    if(isQueued)
    {
      //Do stuff
    }
}
void SyncAndAsyncResult(array<SQLiteRow@> rows, any@ userdata = null)
{
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

void PluginExit()
{
    //Close request queued
    connection.Close();
}
```


# KeyValueStore
- KeyValueStore class is based on SQLite; it automatically handles table creation and provides a fast, simple way to read and write data in key-value format
- sync ve asny is supported
- KeyValueStore content;
  ```angelscript
    //Callback definition for async calls
    void KeyValueStoreCallBack(KeyValueStoreResult@ result, any@ userdata = null)

    //Constructor
    KeyValueStore@ KeyValueStore(string&in file);

    bool Open();
    void Close();
    string& Get(const string &in key) const;
    string& ErrorMessage() const;
    bool Exists(const string &in key);
    bool Delete(const string &in key);
    void Set(const string &in key, const string &in value);
    uint Count();
    uint DeleteAll();
    bool GetAsync(const string &in key, KeyValueStoreCallBack@ callback, any@ userdata = null);
    bool SetAsync(const string &in key, const string &in value, KeyValueStoreCallBack@ callback, any@ userdata = null);
    bool DeleteAsync(const string &in key, KeyValueStoreCallBack@ callback, any@ userdata = null);
    bool DeleteAllAsync(KeyValueStoreCallBack@ callback, any@ userdata = null);
    bool CountAsync(KeyValueStoreCallBack@ callback, any@ userdata = null);
  ```

# KeyValueStoreResult
- The KeyValueStoreResult class represents the outcome of asynchronous tasks executed via KeyValueStore
- All data will be saved.
```angelscript
string& get_Key() const;
string& get_Value() const
//For GetAsync
bool get_Exists() const;
//For CountAsync and DeleteAsync
uint get_Count() const;
```
# Sample Plugin For KeyValueStore
```angelscript
KeyValueStore@ store = KeyValueStore("scripts/plugins/store/mykvstore.db3");
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );
	store.Open();
	KeyValueStoreSyncTest();
	KeyValueStoreAsyncTest();
}
void KeyValueStoreSyncTest()
{
  //Check the key
  bool exists = store.Exists("mykey");

  //Get the key
  string value = store.Get("mykey");

  //Set the key
  store.Set("mykey", "value");

  //Delete
  store.Delete("mykey");

  //Returning all keys count
  uint keysCount = store.Count();

  //Delete all keys
  store.DeleteAll();
}
void KeyValueStoreAsyncTest()
{
    //Get data
    bool queued = store.GetAsync("key", @KeyStoreAsyncResult);
    //Set with custom data
    queued = store.GetAsync("key", "value", @KeyStoreAsyncResult, any(123456));
    //Delete key
    queued = store.DeleteAsync("key", @KeyStoreAsyncResult);
    //Count key
    queued = store.CountAsync(@KeyStoreAsyncResult);
    //Count key
    queued = store.DeleteAllAsync(@KeyStoreAsyncResult);
}
void KeyStoreAsyncResult(KeyValueStoreResult@ result, any@ userdata = null)
{
    string key = result.Key;
    string value = result.Value;
    //For GetAsync
    bool exists = result.Exist;
    //For CountAsync or DeleteAllAsync
    uint count = result.Count;
}
void PluginExit()
{
  store.Close();
}
```

