#include <svenenhancer.h>
#include <sqlite3_sven.h>
#ifdef WIN32
HMODULE sqliteHandle;
#else
void* sqliteHandle = nullptr;
#endif
void RegisterSqlite()
{
#ifdef WIN32
	sqliteHandle = GetModuleHandleA("sqlite3.dll");
	if (!sqliteHandle)
	{
		sqliteHandle = LoadLibraryA("svencoop\\sqlite3.dll");
	}
#else
	sqliteHandle = dlopen("libsqlite3.so", RTLD_NOLOAD);
	if (!sqliteHandle)
		sqliteHandle = dlopen("svencoop/dlls/libsqlite3.so", RTLD_NOW | RTLD_GLOBAL);

#endif 
	if (sqliteHandle)
	{
		Sqlite3Fn._sqlite3_open = GetSymbol<decltype(&sqlite3_open)>(sqliteHandle, "sqlite3_open", 0x000b0886);
		Sqlite3Fn._sqlite3_open_v2 = GetSymbol<decltype(&sqlite3_open_v2)>(sqliteHandle, "sqlite3_open_v2", 0x000b08a0);
		Sqlite3Fn._sqlite3_close = GetSymbol<decltype(&sqlite3_close)>(sqliteHandle, "sqlite3_close", 0x0008149e);
		Sqlite3Fn._sqlite3_free = GetSymbol<decltype(&sqlite3_free)>(sqliteHandle, "sqlite3_free", 0x0001fca4);
		Sqlite3Fn._sqlite3_exec = GetSymbol<decltype(&sqlite3_exec)>(sqliteHandle, "sqlite3_exec", 0x000900d0);
		Sqlite3Fn._sqlite3_finalize = GetSymbol<decltype(&sqlite3_finalize)>(sqliteHandle, "sqlite3_finalize", 0x00082eac);
		Sqlite3Fn._sqlite3_step = GetSymbol<decltype(&sqlite3_step)>(sqliteHandle, "sqlite3_step", 0x0008eb5e);
		Sqlite3Fn._sqlite3_column_text = GetSymbol<decltype(&sqlite3_column_text)>(sqliteHandle, "sqlite3_column_text", 0x0003b7e6);
		Sqlite3Fn._sqlite3_column_int = GetSymbol<decltype(&sqlite3_column_int)>(sqliteHandle, "sqlite3_column_int", 0x0003007e);
		Sqlite3Fn._sqlite3_column_name = GetSymbol<decltype(&sqlite3_column_name)>(sqliteHandle, "sqlite3_column_name", 0x00017880);
		Sqlite3Fn._sqlite3_errmsg = GetSymbol<decltype(&sqlite3_errmsg)>(sqliteHandle, "sqlite3_errmsg", 0x00042a20);
		Sqlite3Fn._sqlite3_prepare = GetSymbol<decltype(&sqlite3_prepare)>(sqliteHandle, "sqlite3_prepare", 0x000a9584);
		Sqlite3Fn._sqlite3_prepare_v2 = GetSymbol<decltype(&sqlite3_prepare_v2)>(sqliteHandle, "sqlite3_prepare_v2", 0x000a97ac);
		Sqlite3Fn._sqlite3_bind_text =
			GetSymbol<decltype(&sqlite3_bind_text)>(sqliteHandle, "sqlite3_bind_text", 0x00043cb8);
		Sqlite3Fn._sqlite3_bind_int =
			GetSymbol<decltype(&sqlite3_bind_int)>(sqliteHandle, "sqlite3_bind_int", 0x00043e92);
		Sqlite3Fn._sqlite3_bind_double =
			GetSymbol<decltype(&sqlite3_bind_double)>(sqliteHandle, "sqlite3_bind_double", 0x00043d8a);
		Sqlite3Fn._sqlite3_bind_int64 =
			GetSymbol<decltype(&sqlite3_bind_int64)>(sqliteHandle, "sqlite3_bind_int64", 0x00043e06);
		Sqlite3Fn._sqlite3_bind_null =
			GetSymbol<decltype(&sqlite3_bind_null)>(sqliteHandle, "sqlite3_bind_null", 0x00043ebc);
		Sqlite3Fn._sqlite3_bind_parameter_count =
			GetSymbol<decltype(&sqlite3_bind_parameter_count)>(sqliteHandle, "sqlite3_bind_parameter_count", 0x00017918);
		Sqlite3Fn._sqlite3_bind_parameter_index =
			GetSymbol<decltype(&sqlite3_bind_parameter_index)>(sqliteHandle, "sqlite3_bind_parameter_index", 0x0001e4fc);
		Sqlite3Fn._sqlite3_bind_parameter_name =
			GetSymbol<decltype(&sqlite3_bind_parameter_name)>(sqliteHandle, "sqlite3_bind_parameter_name", 0x0001792a);
		Sqlite3Fn._sqlite3_bind_text16 =
			GetSymbol<decltype(&sqlite3_bind_text16)>(sqliteHandle, "sqlite3_bind_text16", 0x00043d5e);
		Sqlite3Fn._sqlite3_bind_value =
			GetSymbol<decltype(&sqlite3_bind_value)>(sqliteHandle, "sqlite3_bind_value", 0x00043ffe);
		Sqlite3Fn._sqlite3_mprintf =
			GetSymbol<decltype(&sqlite3_mprintf)>(sqliteHandle, "sqlite3_mprintf", 0x00057c6a);
		Sqlite3Fn._sqlite3_busy_timeout =
			GetSymbol<decltype(&sqlite3_busy_timeout)>(sqliteHandle, "sqlite3_busy_timeout", 0x00027f1c);
		Sqlite3Fn._sqlite3_changes =
			GetSymbol<decltype(&sqlite3_changes)>(sqliteHandle, "sqlite3_changes", 0x0001b214);
		Sqlite3Fn._sqlite3_column_type = GetSymbol<decltype(&sqlite3_column_type)>(sqliteHandle, "sqlite3_column_type", 0x0003010a);
		Sqlite3Fn._sqlite3_column_double = GetSymbol<decltype(&sqlite3_column_double)>(sqliteHandle, "sqlite3_column_double", 0x00030052);
		Sqlite3Fn._sqlite3_column_count = GetSymbol<decltype(&sqlite3_column_count)>(sqliteHandle, "sqlite3_column_count", 0x000177c2);
		Sqlite3Fn._sqlite3_column_int64 = GetSymbol<decltype(&sqlite3_column_int64)>(sqliteHandle, "sqlite3_column_int64", 0x000300a8);
		Sqlite3Fn._sqlite3_close_v2 = GetSymbol<decltype(&sqlite3_close_v2)>(sqliteHandle, "sqlite3_close_v2", 0x000814aa);

		Sqlite3Fn._sqlite3_bind_blob = GetSymbol<decltype(&sqlite3_bind_blob)>(sqliteHandle, "sqlite3_bind_blob", 0x00043c32);
		Sqlite3Fn._sqlite3_blob_open = GetSymbol<decltype(&sqlite3_blob_open)>(sqliteHandle, "sqlite3_blob_open", 0x0009aad4);
		Sqlite3Fn._sqlite3_blob_close = GetSymbol<decltype(&sqlite3_blob_close)>(sqliteHandle, "sqlite3_blob_close", 0x00082f96);
		Sqlite3Fn._sqlite3_column_blob = GetSymbol<decltype(&sqlite3_column_blob)>(sqliteHandle, "sqlite3_column_blob", 0x0003b468);
		Sqlite3Fn._sqlite3_blob_read = GetSymbol<decltype(&sqlite3_blob_read)>(sqliteHandle, "sqlite3_blob_read", 0x00083218);
		Sqlite3Fn._sqlite3_blob_write = GetSymbol<decltype(&sqlite3_blob_write)>(sqliteHandle, "sqlite3_blob_write", 0x00083246);
		Sqlite3Fn._sqlite3_blob_bytes = GetSymbol<decltype(&sqlite3_blob_bytes)>(sqliteHandle, "sqlite3_blob_bytes", 0x00017b74);
		Sqlite3Fn._sqlite3_bind_zeroblob = GetSymbol<decltype(&sqlite3_bind_zeroblob)>(sqliteHandle, "sqlite3_bind_zeroblob", 0x00043ee6);
		Sqlite3Fn._sqlite3_blob_reopen = GetSymbol<decltype(&sqlite3_blob_reopen)>(sqliteHandle, "sqlite3_blob_reopen", 0x0008ffc0);

		Sqlite3Fn._sqlite3_value_blob = GetSymbol<decltype(&sqlite3_value_blob)>(sqliteHandle, "sqlite3_value_blob", 0x0003b308);
		Sqlite3Fn._sqlite3_result_blob = GetSymbol<decltype(&sqlite3_result_blob)>(sqliteHandle, "sqlite3_result_blob", 0x0003361e);
		Sqlite3Fn._sqlite3_result_zeroblob = GetSymbol<decltype(&sqlite3_result_zeroblob)>(sqliteHandle, "sqlite3_result_zeroblob", 0x0002fcca);

		Sqlite3Fn.loaded = Sqlite3Fn._sqlite3_open && Sqlite3Fn._sqlite3_close && Sqlite3Fn._sqlite3_exec && Sqlite3Fn._sqlite3_mprintf;
		if (Sqlite3Fn.loaded)
		{
			//StartSqlite Worker
			static std::thread worker(SqliteWorker);
			worker.detach();
		}
	}

}

void RegisterSqliteAngelScript(asIScriptEngine* engine)
{
	engine->RegisterEnum("SQLITE_OPEN_FLAG");
	engine->RegisterEnumValue("SQLITE_OPEN_FLAG", "SQLITE_OPEN_READWRITE", SQLITE_OPEN_READWRITE);
	engine->RegisterEnumValue("SQLITE_OPEN_FLAG", "SQLITE_OPEN_READONLY", SQLITE_OPEN_READONLY);
	engine->RegisterEnumValue("SQLITE_OPEN_FLAG", "SQLITE_OPEN_CREATE", SQLITE_OPEN_CREATE);
	engine->RegisterEnum("SQLITE_TYPE");
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_INVALID_TYPE", -1);
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_INTEGER", SQLITE_INTEGER);
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_FLOAT", SQLITE_FLOAT);
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_TEXT", SQLITE_TEXT);
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_BLOB", SQLITE_BLOB);
	engine->RegisterEnumValue("SQLITE_TYPE", "SQLITE_NULL", SQLITE_NULL);


	RegisterObject<SqliteRow>("SQLiteRow", engine, asOBJ_REF | asOBJ_GC);
	RegisterObject<SqliteConnection>("SQLiteConnection", engine, asOBJ_REF | asOBJ_GC);
	RegisterObject<KeyValueStore>("KeyValueStore", engine, asOBJ_REF | asOBJ_GC);
	engine->RegisterObjectBehaviour("KeyValueStore", asBEHAVE_FACTORY, "KeyValueStore@ KeyValueStore(string&in file)", asFunctionPtr(KeyValueStore::Factory), asCALL_CDECL);
	engine->RegisterObjectType("KeyValueStoreResult", 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectMethod("SQLiteRow", "string& GetString(string&in column) const", asMETHOD(SqliteRow, GetString), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "int GetInt(string&in column) const", asMETHOD(SqliteRow, GetInt), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "double GetDouble(string&in column) const", asMETHOD(SqliteRow, GetDouble), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "float GetFloat(string&in column) const", asMETHOD(SqliteRow, GetFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "bool GetBool(string&in column) const", asMETHOD(SqliteRow, GetBool), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "string& GetColumnName(uint index) const", asMETHOD(SqliteRow, GetColumnName), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "int ColumnsCount() const", asMETHOD(SqliteRow, ColumnsCount), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "bool IsNull() const", asMETHOD(SqliteRow, IsNull), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteRow", "SQLITE_TYPE GetColumnType(string&in column) const", asMETHOD(SqliteRow, ColumnType), asCALL_THISCALL);

	engine->RegisterObjectBehaviour("SQLiteConnection", asBEHAVE_FACTORY, "SQLiteConnection@ SQLiteConnection()", asFunctionPtr(SqliteConnection::Factory), asCALL_CDECL);
	engine->RegisterObjectMethod("SQLiteConnection", "string& get_File() const", asMETHOD(SqliteConnection, GetFile), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection", "void set_File(string& in file)", asMETHOD(SqliteConnection, SetFile), asCALL_THISCALL);
	engine->RegisterObjectProperty("SQLiteConnection", "SQLITE_OPEN_FLAG Flags", offsetof(SqliteConnection, flags), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection", "bool Open() const", asMETHOD(SqliteConnection, Open), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection", "void Close()", asMETHOD(SqliteConnection, Close), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection", "bool IsOpened() const", asMETHOD(SqliteConnection, IsOpened), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection", "string& ErrorMessage()", asMETHOD(SqliteConnection, ErrorMsg), asCALL_THISCALL);

	engine->RegisterFuncdef("void SqliteResultCallback(array<SQLiteRow@> rows, any@ userdata = null)");

	engine->RegisterObjectMethod("SQLiteConnection",
		"array<SQLiteRow@>@ Execute(const string &in query, dictionary@ params = null)",
		asMETHOD(SqliteConnection, ExecuteResult), asCALL_THISCALL);

	engine->RegisterObjectMethod("SQLiteConnection",
		"bool ExecuteAsync(const string &in query, SqliteResultCallback@ callback, any@ userdata = null)",
		asMETHOD(SqliteConnection, ExecuteResultAsync), asCALL_THISCALL);
	engine->RegisterObjectMethod("SQLiteConnection",
		"bool ExecuteAsync(const string &in query, dictionary @params, SqliteResultCallback@ callback, any@ userdata = null)",
		asMETHOD(SqliteConnection, ExecuteResultAsyncWithParams), asCALL_THISCALL);

	engine->RegisterFuncdef("void KeyValueStoreCallBack(KeyValueStoreResult@ result, any@ userdata = null)");

	engine->RegisterObjectMethod("KeyValueStore",
		"bool Open()",
		asMETHOD(KeyValueStore, Open), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"void Close()",
		asMETHOD(KeyValueStore, Close), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"string& Get(const string &in key) const",
		asMETHOD(KeyValueStore, Get), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"string& ErrorMessage() const",
		asMETHOD(KeyValueStore, ErrorMsg), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool Exists(const string &in key)",
		asMETHOD(KeyValueStore, Exists), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool Delete(const string &in key)",
		asMETHOD(KeyValueStore, Delete), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"void Set(const string &in key, const string &in value)",
		asMETHOD(KeyValueStore, Set), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"uint Count()",
		asMETHOD(KeyValueStore, GetCount), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"uint DeleteAll()",
		asMETHOD(KeyValueStore, DeleteAll), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool GetAsync(const string &in key, KeyValueStoreCallBack@ callback, any@ userdata = null)",
		asMETHOD(KeyValueStore, GetAsync), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool SetAsync(const string &in key, const string &in value, KeyValueStoreCallBack@ callback, any@ userdata = null)",
		asMETHOD(KeyValueStore, SetAsync), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool DeleteAsync(const string &in key, KeyValueStoreCallBack@ callback, any@ userdata = null)",
		asMETHOD(KeyValueStore, DeleteAsync), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool DeleteAllAsync(KeyValueStoreCallBack@ callback, any@ userdata = null)",
		asMETHOD(KeyValueStore, DeleteAllAsync), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStore",
		"bool CountAsync(KeyValueStoreCallBack@ callback, any@ userdata = null)",
		asMETHOD(KeyValueStore, CountAsync), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStoreResult",
		"string& get_Key() const",
		asMETHOD(KeyValueStoreResult, GetKey), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStoreResult",
		"string& get_Value() const",
		asMETHOD(KeyValueStoreResult, GetValue), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStoreResult",
		"bool get_Exists() const",
		asMETHOD(KeyValueStoreResult, GetExists), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyValueStoreResult",
		"uint get_Count() const",
		asMETHOD(KeyValueStoreResult, Count), asCALL_THISCALL);

}
