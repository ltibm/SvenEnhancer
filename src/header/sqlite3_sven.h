#pragma once
#include <sqlite3.h>
#include <xxhash.h>
#include <thread>
#ifndef WIN32
#include <filesystem>
#endif

#ifdef WIN32
extern HMODULE sqliteHandle;
#else
extern void* sqliteHandle;
#endif
void RegisterSqlite();
void RegisterSqliteAngelScript(asIScriptEngine* engine);

struct Sqlite3
{
	bool loaded;
	decltype(&sqlite3_open) _sqlite3_open;
	decltype(&sqlite3_open_v2) _sqlite3_open_v2;
	decltype(&sqlite3_close) _sqlite3_close;
	decltype(&sqlite3_free) _sqlite3_free;
	decltype(&sqlite3_exec) _sqlite3_exec;
	decltype(&sqlite3_finalize) _sqlite3_finalize;
	decltype(&sqlite3_step) _sqlite3_step;
	decltype(&sqlite3_column_text) _sqlite3_column_text;
	decltype(&sqlite3_column_int) _sqlite3_column_int;
	decltype(&sqlite3_column_name) _sqlite3_column_name;
	decltype(&sqlite3_errmsg) _sqlite3_errmsg;
	decltype(&sqlite3_prepare) _sqlite3_prepare;
	decltype(&sqlite3_prepare_v2) _sqlite3_prepare_v2;
	decltype(&sqlite3_bind_text) _sqlite3_bind_text;
	decltype(&sqlite3_bind_int) _sqlite3_bind_int;
	decltype(&sqlite3_bind_double) _sqlite3_bind_double;
	decltype(&sqlite3_bind_int64) _sqlite3_bind_int64;
	decltype(&sqlite3_bind_null) _sqlite3_bind_null;
	decltype(&sqlite3_bind_parameter_count) _sqlite3_bind_parameter_count;
	decltype(&sqlite3_bind_parameter_index) _sqlite3_bind_parameter_index;
	decltype(&sqlite3_bind_parameter_name) _sqlite3_bind_parameter_name;
	decltype(&sqlite3_bind_text16) _sqlite3_bind_text16;
	decltype(&sqlite3_bind_value) _sqlite3_bind_value;
	decltype(&sqlite3_mprintf) _sqlite3_mprintf;
	decltype(&sqlite3_busy_timeout) _sqlite3_busy_timeout;
	decltype(&sqlite3_changes) _sqlite3_changes;
	decltype(&sqlite3_column_type) _sqlite3_column_type;
	decltype(&sqlite3_column_double) _sqlite3_column_double;
	decltype(&sqlite3_column_count) _sqlite3_column_count;
	decltype(&sqlite3_column_int64) _sqlite3_column_int64;
	decltype(&sqlite3_close_v2) _sqlite3_close_v2;

	decltype(&sqlite3_bind_blob) _sqlite3_bind_blob;
	decltype(&sqlite3_blob_open) _sqlite3_blob_open;
	decltype(&sqlite3_blob_close) _sqlite3_blob_close;
	decltype(&sqlite3_column_blob) _sqlite3_column_blob;
	decltype(&sqlite3_blob_read) _sqlite3_blob_read;
	decltype(&sqlite3_blob_write) _sqlite3_blob_write;
	decltype(&sqlite3_blob_bytes) _sqlite3_blob_bytes;
	decltype(&sqlite3_bind_zeroblob) _sqlite3_bind_zeroblob;
	decltype(&sqlite3_blob_reopen) _sqlite3_blob_reopen;

	decltype(&sqlite3_value_blob) _sqlite3_value_blob;
	decltype(&sqlite3_result_blob) _sqlite3_result_blob;
	decltype(&sqlite3_result_zeroblob) _sqlite3_result_zeroblob;


};
inline std::mutex g_SqliteRequestMutex;
inline std::mutex g_SqliteResponseMutex;
inline std::mutex g_SqliteConnectionMutex;

inline Sqlite3 Sqlite3Fn;
inline bool sqliteKeepRun = true;

struct SqliteCell {
	int type;
	union {
		long long iVal;
		double dVal;
	} data;
	std::string sVal;
	std::string paramName;
	SqliteCell() : type(SQLITE_NULL
	), sVal(""), paramName("") {
		data.iVal = 0;
	}
};
class SqliteRow;
struct SqliteRequestItem {
	sqlite3* db;
	asIScriptFunction* callback;
	std::vector<SqliteCell> params;
	std::string query;
	std::string paramkey;
	std::string paramvalue;
	void* userData;
	bool isCompleted;
	bool isKeyStore;
	int keyStoreType = 0; //0: Get, 1: Update, 2: Delete, 3: Exists
	SqliteRequestItem() {

	}
};
struct SqliteResultItem {
	asIScriptFunction* callback;
	sqlite3* db;
	void* userData;
	std::vector<SqliteRow> results;
	bool isKeyStore;
	std::string paramkey;
	std::string paramvalue;
	size_t paramcount = 0;
	bool paramExists;
	int keyStoreType = 0; //0: Get, 1: Update, 2: Delete, 3: Exists

	SqliteResultItem() {

	};
};
struct SqliteConnectionItem {
	sqlite3* db;
	asIScriptModule* module;
	bool markAsClosed = false;

};

inline std::vector<SqliteConnectionItem> g_SqliteConnections;
inline std::queue<SqliteResultItem> g_SqliteResultQueues;
inline std::queue<SqliteRequestItem> g_SqliteRequestQueues;


class SqliteRow : public CASBaseGCObject {
public:

	std::vector<std::string> columns;
	std::unordered_map<std::string, SqliteCell> cells;

	const std::string& GetColumnByIndex(size_t index) {
		static const std::string _empty = "";
		if (index >= columns.size())
			return _empty;
		return columns[index];
	}
	CString* GetColumnName(size_t index)
	{
		if (index >= columns.size())
			return CreateString("");
		return CreateString(columns[index].c_str());
	}
	size_t ColumnsCount()
	{
		return columns.size();
	}
	int ColumnType(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it == cells.end())
			return -1;
		return it->second.type;
	}
	int ColumnTypeI(int index)
	{
		auto column = GetColumnByIndex(index);
		if (column.empty())
			return -1;
		auto it = cells.find(column.c_str());
		if (it == cells.end())
			return -1;
		return it->second.type;
	}
	bool IsNull(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return it->second.type == SQLITE_NULL;
		return false;
	}
	bool GetBool(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return it->second.data.iVal > 0;
		return false;
	}
	int GetInt(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return it->second.data.iVal;
		return {};
	}
	size_t GetUInt(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return it->second.data.iVal;
		return {};
	}
	float GetFloat(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return static_cast<float>(it->second.data.dVal);
		return {};
	}
	double GetDouble(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return static_cast<double>(it->second.data.dVal);
		return {};
	}
	CString* GetStringI(size_t index)
	{
		auto column = GetColumnByIndex(index);
		if (column.empty())
			return CreateString("");
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return CreateString(it->second.sVal.c_str());
		return CreateString("");
	}
	CString* GetString(CString& column)
	{
		auto it = cells.find(column.c_str());
		if (it != cells.end())
			return CreateString(it->second.sVal.c_str());
		return CreateString("");
	}
	void ReadAllColumns(sqlite3_stmt* stmt) {
		int colCount = Sqlite3Fn._sqlite3_column_count(stmt);
		for (int i = 0; i < colCount; i++) {
			AddColumn(stmt, i);
		}
	}
	void AddColumn(sqlite3_stmt* stmt, int colIndex) {
		const char* name = Sqlite3Fn._sqlite3_column_name(stmt, colIndex);
		columns.push_back(name);
		int type = Sqlite3Fn._sqlite3_column_type(stmt, colIndex);
		SqliteCell val;
		val.type = type;
		switch (type) {
		case SQLITE_INTEGER:
			val.data.iVal = Sqlite3Fn._sqlite3_column_int64(stmt, colIndex);
			val.sVal = std::to_string(val.data.iVal);
			break;
		case SQLITE_FLOAT:
			val.data.dVal = Sqlite3Fn._sqlite3_column_double(stmt, colIndex);
			val.sVal = std::to_string(val.data.dVal);
			break;
		case SQLITE_TEXT:
		{
			const char* txt = (const char*)Sqlite3Fn._sqlite3_column_text(stmt, colIndex);
			val.sVal = txt ? txt : "";
			break;
		}
		case SQLITE_NULL:
			val.sVal = "NULL";
			break;
		}
		cells[name] = val;
	}
	static SqliteRow* Factory() {
		SqliteRow* obj = new SqliteRow();
		CASServerManager* manager = ASEXT_GetServerManager();
		asIScriptEngine* engine = manager->scriptEngine;
		static asITypeInfo* typeR = nullptr;
		typeR = typeR ? typeR : engine->GetTypeInfoByName("SQLiteRow");
		engine->NotifyGarbageCollectorOfNewObject(obj, typeR);
		return obj;
	}

};
struct KeyValueStoreResult
{
	std::string key;
	std::string value;
	bool exists;
	int count;
	CString* GetKey()
	{
		return CreateString(key.c_str());
	};
	CString* GetValue()
	{
		return CreateString(value.c_str());
	};
	bool GetExists()
	{
		return exists;
	}
	size_t Count()
	{
		return count;
	}
};
class SqliteConnection : public CASBaseGCObject {
public:
	static void CallbackFrame()
	{
		auto engine = GetASEngine();
		while (!g_SqliteResultQueues.empty())
		{
			SqliteResultItem responseData;
			{
				std::lock_guard<std::mutex> lock(g_SqliteResponseMutex);
				responseData = g_SqliteResultQueues.front();
				g_SqliteResultQueues.pop();
			}
			if (responseData.callback && IsConnectionValid(responseData.db))
			{
				if (responseData.isKeyStore)
				{
					if (responseData.keyStoreType == 0 || responseData.keyStoreType == 1 || responseData.keyStoreType == 2 || responseData.keyStoreType == 3 || responseData.keyStoreType == 4 || responseData.keyStoreType == 5) //get, set, delete, deleteall, count
					{
						KeyValueStoreResult result;
						result.key = responseData.paramkey;
						result.value = responseData.paramvalue;
						result.exists = responseData.paramExists;
						result.count = responseData.paramcount;
						auto ctx = engine->RequestContext();
						ctx->Prepare(responseData.callback);
						ctx->SetArgObject(0, &result);
						ctx->SetArgObject(1, responseData.userData);
						ctx->Execute();
						engine->ReturnContext(ctx);
					}
				}
				else {
					auto ctx = engine->RequestContext();
					ctx->Prepare(responseData.callback);
					auto results = SqliteConnection::GetArrayFromResult(responseData);
					ctx->SetArgObject(0, results);
					ctx->SetArgObject(1, responseData.userData);
					ctx->Execute();
					engine->ReturnContext(ctx);
				}

			}
		}
	}
	static bool IsConnectionValid(sqlite3* db)
	{
		std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
		auto it = std::find_if(g_SqliteConnections.begin(), g_SqliteConnections.end(), [db](const SqliteConnectionItem& item) {
			return item.db == db;
			});
		if (it == g_SqliteConnections.end())
			return false;
		return !it->markAsClosed && it->db;
	}

	static void AddToConnection(sqlite3* db)
	{
		SqliteConnectionItem cnitem;
		cnitem.db = db;
		cnitem.module = GetActiveModule();
		{
			std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
			g_SqliteConnections.push_back(cnitem);
		}
	}
	SqliteConnection()
	{
		db = nullptr;
		fileName = {};
		flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
		_isopened = false;
	}
	sqlite3* db;
	std::string fileName;
	int flags;
	bool _isopened;
	inline void SetFile(CString& name)
	{
		fileName = name.c_str();
	}
	inline CString* GetFile()
	{
		return CreateString(fileName.c_str());
	}

	inline bool Open()
	{
		if (!Sqlite3Fn.loaded || IsOpened() || fileName.empty())
			return false;

		std::string jsonString = fileName.c_str();
		if (jsonString.ends_with(".as"))
		{
			return false;
		}
		std::filesystem::path _path(jsonString);
		std::filesystem::path rootPath = "./svencoop";
		std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
		if (Sqlite3Fn._sqlite3_open_v2(full.string().c_str(), &db, flags, nullptr) != SQLITE_OK)
		{
			Sqlite3Fn._sqlite3_close(db);
			return false;
		}
		_isopened = true;
		AddToConnection(db);
		return true;
	}
	inline void Close()
	{
		if (IsOpened())
		{
			auto aranan = db;
			std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
			auto it = std::find_if(g_SqliteConnections.begin(), g_SqliteConnections.end(), [aranan](const SqliteConnectionItem& item) {
				return item.db == aranan;
				});
			if (it != g_SqliteConnections.end())
			{
				it->markAsClosed = true;
			}
		}
		_isopened = false;
	}
	inline bool IsOpened()
	{
		return db && _isopened;
	}
	CString* ErrorMsg()
	{
		CString* str = new CString();
		str->assign("", 0);
		if (db)
		{
			const char* err = Sqlite3Fn._sqlite3_errmsg(db);
			if (err)
			{
				str->assign(err, strlen(err));
			}
		}
		return str;
	}
	bool ExecuteResultAsync(CString& query, asIScriptFunction* callback, void* userdata)
	{
		return ExecuteResultAsyncWithParams(query, nullptr, callback, userdata);
	}

	//Async
	bool ExecuteResultAsyncWithParams(CString& query, CScriptDictionary* parameters, asIScriptFunction* callback, void* userdata)
	{
		if (!this->IsOpened() || query.empty())
			return false;
		SqliteRequestItem item;
		item.query = query.c_str();
		item.callback = callback;
		item.params = DictToCell(parameters);
		item.userData = userdata;
		item.db = db;
		std::lock_guard<std::mutex> lock(g_SqliteRequestMutex);
		g_SqliteRequestQueues.push(item);
		return true;
	}


	static void CloseAllConnectionsByModule(asIScriptModule* module)
	{
		std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
		for (auto& item : g_SqliteConnections)
		{
			if (item.module == module)
				item.markAsClosed = true;
			item.module = nullptr;
		}
	}


	static CScriptArray* GetArrayFromResult(SqliteResultItem& result)
	{
		auto eng = GetASEngine();
		static asITypeInfo* arrInfoRow = eng->GetTypeInfoByDecl("array<SQLiteRow@>");
		int typeId = arrInfoRow->GetTypeId();
		auto arr = eng->CreateScriptObject(arrInfoRow);
		eng->NotifyGarbageCollectorOfNewObject(arr, arrInfoRow);
		auto method = arrInfoRow->GetMethodByName("insertLast");
		if (!method)
		{
			return (CScriptArray*)arr;
		}
		sqlite3_stmt* stmt;
		auto type = eng->GetTypeInfoByName("SQLiteRow");;
		auto ctx = eng->RequestContext();

		for (auto& row : result.results)
		{
			SqliteRow* _row = new SqliteRow(row);
			ctx->Prepare(method);
			ctx->SetObject(arr);
			ctx->SetArgObject(0, &_row);
			ctx->Execute();
		}
		eng->ReturnContext(ctx);
		return (CScriptArray*)arr;
	}

	//Sync
	CScriptArray* ExecuteResult(CString& query, CScriptDictionary* parameters)
	{
		auto eng = GetASEngine();
		static asITypeInfo* arrInfoRow = eng->GetTypeInfoByDecl("array<SQLiteRow@>");
		int typeId = arrInfoRow->GetTypeId();
		auto arr = eng->CreateScriptObject(arrInfoRow);
		eng->NotifyGarbageCollectorOfNewObject(arr, arrInfoRow);
		auto method = arrInfoRow->GetMethodByName("insertLast");
		if (!method || !IsOpened() || !Sqlite3Fn.loaded)
		{
			return (CScriptArray*)arr;
		}
		sqlite3_stmt* stmt;
		if (Sqlite3Fn._sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
			if (parameters)
				FillParametersFromDictionary(stmt, parameters);
			while (Sqlite3Fn._sqlite3_step(stmt) == SQLITE_ROW) {
				SqliteRow* row = SqliteRow::Factory();
				row->ReadAllColumns(stmt);
				auto ctx = eng->RequestContext();
				ctx->Prepare(method);
				ctx->SetObject(arr);
				ctx->SetArgAddress(0, &row);
				ctx->Execute();
				eng->ReturnContext(ctx);
			}
			Sqlite3Fn._sqlite3_finalize(stmt);
		}
		return (CScriptArray*)arr;
	}
	static void FillParametersFromVec(sqlite3_stmt* stmt, std::vector<SqliteCell> parameters)
	{
		if (!parameters.size())
			return;
		for (auto& item : parameters)
		{
			if (item.paramName.empty())
				continue;
			auto pindex = Sqlite3Fn._sqlite3_bind_parameter_index(stmt, item.paramName.c_str());
			if (pindex > 0)
			{
				auto typeId = item.type;
				if (typeId == asTYPEID_FLOAT || typeId == asTYPEID_DOUBLE)
				{
					Sqlite3Fn._sqlite3_bind_double(stmt, pindex, item.data.dVal);
				}
				else if (typeId == asTYPEID_INT64)
				{
					Sqlite3Fn._sqlite3_bind_int64(stmt, pindex, item.data.iVal);
				}
				else if (isNumericType(typeId) || typeId == asTYPEID_BOOL)
				{
					Sqlite3Fn._sqlite3_bind_int(stmt, pindex, item.data.iVal);
				}
				else if (typeId == AS_TYPEID_STRING)
				{
					Sqlite3Fn._sqlite3_bind_text(stmt, pindex, item.sVal.c_str(), -1, SQLITE_TRANSIENT);
				}
				else if (typeId == AS_TYPEID_DATETIME)
				{
					Sqlite3Fn._sqlite3_bind_int64(stmt, pindex, item.data.iVal);
				}
			}
		}
	}
	static void FillParametersFromDictionary(sqlite3_stmt* stmt, CScriptDictionary* parameters)
	{
		if (!parameters)
			return;
		CDictHelper* helpers = new CDictHelper(parameters);
		auto keys = helpers->getKeysV2();
		for (auto& k : keys)
		{
			auto dictValue = helpers->getByName(k);
			if (dictValue)
			{
				auto typeId = dictValue->GetTypeId();
				if (isPrimviteType(typeId))
				{
					std::string _paramName = "@" + k;
					auto pindex = Sqlite3Fn._sqlite3_bind_parameter_index(stmt, _paramName.c_str());
					if (pindex > 0)
					{
						if (typeId == asTYPEID_FLOAT || typeId == asTYPEID_DOUBLE)
						{
							Sqlite3Fn._sqlite3_bind_double(stmt, pindex, dictValue->m_valueFlt);
						}
						else if (typeId == asTYPEID_INT64)
						{
							Sqlite3Fn._sqlite3_bind_int64(stmt, pindex, dictValue->m_valueInt);
						}
						else if (isNumericType(typeId) || typeId == asTYPEID_BOOL)
						{
							Sqlite3Fn._sqlite3_bind_int(stmt, pindex, dictValue->m_valueInt);
						}
						else if (typeId == AS_TYPEID_STRING)
						{
							CString* strPtr = static_cast<CString*>(dictValue->m_valueObj);
							Sqlite3Fn._sqlite3_bind_text(stmt, pindex, strPtr->c_str(), -1, SQLITE_TRANSIENT);
						}
						else if (typeId == AS_TYPEID_DATETIME)
						{
							auto* dateHelper = new CDateTimeHelper(dictValue->m_valueObj);
							sqlite3_int64 safeTime = static_cast<sqlite3_int64>(dateHelper->ToUnix());
							Sqlite3Fn._sqlite3_bind_int64(stmt, pindex, safeTime);
						}
					}
				}
			}
		}
	}
	static const std::vector<SqliteCell> DictToCell(CScriptDictionary* parameters)
	{
		std::vector<SqliteCell> params;
		if (!parameters)
			return params;
		CDictHelper* helpers = new CDictHelper(parameters);
		auto keys = helpers->getKeysV2();
		for (auto& k : keys)
		{
			auto dictValue = helpers->getByName(k);
			if (dictValue)
			{
				auto typeId = dictValue->GetTypeId();
				if (isPrimviteType(typeId))
				{
					std::string _paramName = "@" + k;
					SqliteCell cell;
					cell.paramName = _paramName;
					cell.type = typeId;
					if (typeId == asTYPEID_FLOAT || typeId == asTYPEID_DOUBLE)
					{
						cell.data.dVal = dictValue->m_valueFlt;
					}
					else if (typeId == asTYPEID_INT64)
					{
						cell.data.iVal = dictValue->m_valueInt;
					}
					else if (isNumericType(typeId) || typeId == asTYPEID_BOOL)
					{
						cell.data.iVal = dictValue->m_valueInt;
					}
					else if (typeId == AS_TYPEID_STRING)
					{
						CString* strPtr = static_cast<CString*>(dictValue->m_valueObj);
						cell.sVal = strPtr->c_str();
					}
					else if (typeId == AS_TYPEID_DATETIME)
					{
						auto* dateHelper = new CDateTimeHelper(dictValue->m_valueObj);
						sqlite3_int64 safeTime = static_cast<sqlite3_int64>(dateHelper->ToUnix());
						cell.data.iVal = safeTime;
					}
					else {
						continue;
					}
					params.push_back(cell);
				}
			}
		}
		return params;
	}

	static SqliteConnection* Factory() {
		SqliteConnection* obj = new SqliteConnection();
		CASServerManager* manager = ASEXT_GetServerManager();
		asIScriptEngine* engine = manager->scriptEngine;
		static asITypeInfo* typesqlite = nullptr;
		typesqlite = typesqlite ? typesqlite : engine->GetTypeInfoByName("SQLiteConnection");
		engine->NotifyGarbageCollectorOfNewObject(obj, typesqlite);
		return obj;
	}
};



class KeyValueStore : public CASBaseGCObject {
public:
	std::string fileName;
	bool _isopened = false;
	sqlite3* db = nullptr;
	static KeyValueStore* Factory(CString& name) {
		KeyValueStore* obj = new KeyValueStore();
		obj->fileName = name.c_str();
		CASServerManager* manager = ASEXT_GetServerManager();
		asIScriptEngine* engine = manager->scriptEngine;
		static asITypeInfo* typekvstore = nullptr;
		typekvstore = typekvstore ? typekvstore : engine->GetTypeInfoByName("KeyValueStore");
		engine->NotifyGarbageCollectorOfNewObject(obj, typekvstore);
		return obj;
	}
	bool Open()
	{
		if (!Sqlite3Fn.loaded || IsOpened() || fileName.empty())
			return false;

		std::string jsonString = fileName.c_str();
		if (jsonString.ends_with(".as"))
		{
			return false;
		}
		std::filesystem::path _path(jsonString);
		std::filesystem::path rootPath = "./svencoop";
		std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
		if (Sqlite3Fn._sqlite3_open_v2(full.string().c_str(), &db,  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
		{
			Sqlite3Fn._sqlite3_close(db);
			return false;
		}
		_isopened = true;
		SqliteConnection::AddToConnection(db);
		//First create table if not exists
		ExecuteResultAsync(R"(
			CREATE TABLE IF NOT EXISTS SE_KvStoreData (
				key TEXT PRIMARY KEY,
				value TEXT,
				created_at INTEGER,
				updated_at INTEGER
			);
		)", nullptr, nullptr, "", "", - 1);
		return true;
	}
	bool ExecuteResultAsync(std::string query, asIScriptFunction* callback, void* userdata, std::string _k, std::string _v, int type)
	{
		if (!this->IsOpened())
			return false;
		SqliteRequestItem item;
		item.query = query.c_str();
		item.callback = callback;
		item.userData = userdata;
		item.db = db;
		item.isKeyStore = true;
		item.paramkey = _k;
		item.paramvalue = _v;
		item.keyStoreType = type;
		std::lock_guard<std::mutex> lock(g_SqliteRequestMutex);
		g_SqliteRequestQueues.push(item);
		return true;
	}
	bool IsOpened()
	{
		return _isopened && db;
	}

	void Delete(CString& key)
	{
		if (!IsOpened())
			return;
		PrepareDelete(db, key.c_str());
	}
	static size_t PrepareDelete(sqlite3* db, std::string key)
	{
		sqlite3_stmt* stmt;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"DELETE FROM SE_KvStoreData WHERE key = ?;",
			-1, &stmt, nullptr);
		Sqlite3Fn._sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

		Sqlite3Fn._sqlite3_step(stmt);
		size_t affected = Sqlite3Fn._sqlite3_changes(db);
		Sqlite3Fn._sqlite3_finalize(stmt);
		return affected;
	}
	static size_t PrepareCount(sqlite3* db)
	{
		sqlite3_stmt* stmt;
		size_t count = 0;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"SELECT COUNT(*) FROM SE_KvStoreData;",
			-1, &stmt, nullptr);
		if (Sqlite3Fn._sqlite3_step(stmt) == SQLITE_ROW)
			count = Sqlite3Fn._sqlite3_column_int(stmt, 0);
		Sqlite3Fn._sqlite3_finalize(stmt);
		return count;
	}

	static size_t PrepareDeleteAll(sqlite3* db)
	{
		sqlite3_stmt* stmt;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"DELETE FROM SE_KvStoreData;",
			-1, &stmt, nullptr);
		Sqlite3Fn._sqlite3_step(stmt);
		size_t affected = Sqlite3Fn._sqlite3_changes(db);
		Sqlite3Fn._sqlite3_finalize(stmt);
		return affected;
	}
	bool Exists(CString& key)
	{
		if (!IsOpened())
			return false;
		return PrepareExists(db, key.c_str());
	}
	static bool PrepareExists(sqlite3* db, std::string key)
	{
		sqlite3_stmt* stmt;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"SELECT 1 FROM SE_KvStoreData WHERE key = ? LIMIT 1;",
			-1, &stmt, nullptr);
		Sqlite3Fn._sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
		bool found = (Sqlite3Fn._sqlite3_step(stmt) == SQLITE_ROW);
		Sqlite3Fn._sqlite3_finalize(stmt);
		return found;
	}
	inline CString* Get(CString& key)
	{
		if (!IsOpened())
			return CreateString("");
		bool exists = false;
		return CreateString(PrepareGet(db, key.c_str(), exists).c_str());
	}
	inline static std::string PrepareGet(sqlite3* db, std::string key, bool &exists)
	{
		std::string result;
		sqlite3_stmt* stmt;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"SELECT value FROM SE_KvStoreData WHERE key=?;",
			-1, &stmt, nullptr);
		Sqlite3Fn._sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
		exists = false;
		if (Sqlite3Fn._sqlite3_step(stmt) == SQLITE_ROW)
		{
			result = (const char*)Sqlite3Fn._sqlite3_column_text(stmt, 0);
			exists = true;
		}
		Sqlite3Fn._sqlite3_finalize(stmt);
		return result;
	}
	size_t GetCount()
	{
		if (!IsOpened())
			return 0;
		return PrepareCount(db);
	}
	size_t DeleteAll()
	{
		if (!IsOpened())
			return 0;
		return PrepareDeleteAll(db);
	}
	bool GetAsync(CString& key, asIScriptFunction* callback, void* userData)
	{
		return ExecuteResultAsync("", callback, userData, key.c_str(), "", 0);
	}
	bool SetAsync(CString& key, CString& value, asIScriptFunction* callback, void* userData)
	{
		return ExecuteResultAsync("", callback, userData, key.c_str(), value.c_str(), 1);
	}
	bool DeleteAsync(CString& key, asIScriptFunction* callback, void* userData)
	{
		return ExecuteResultAsync("", callback, userData, key.c_str(), "", 2);
	}
	bool DeleteAllAsync(asIScriptFunction* callback, void* userData)
	{
		return ExecuteResultAsync("", callback, userData, "", "", 3);
	}
	bool CountAsync(asIScriptFunction* callback, void* userData)
	{
		return ExecuteResultAsync("", callback, userData, "", "", 4);
	}
	void Set(CString& key, CString& value)
	{
		if (!IsOpened())
			return;
		PrepareSet(db, key.c_str(), value.c_str());

	}
	CString* ErrorMsg()
	{
		CString* str = new CString();
		str->assign("", 0);
		if (db)
		{
			const char* err = Sqlite3Fn._sqlite3_errmsg(db);
			if (err)
			{
				str->assign(err, strlen(err));
			}
		}
		return str;
	}
	static void PrepareSet(sqlite3* db, std::string key, std::string value)
	{
		int now = (int)time(nullptr);
		sqlite3_stmt* stmt;
		Sqlite3Fn._sqlite3_prepare_v2(db,
			"UPDATE SE_KvStoreData SET value=?, updated_at=? WHERE key=?;",
			-1, &stmt, nullptr);
		Sqlite3Fn._sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_TRANSIENT);
		Sqlite3Fn._sqlite3_bind_int(stmt, 2, now);
		Sqlite3Fn._sqlite3_bind_text(stmt, 3, key.c_str(), -1, SQLITE_TRANSIENT);
		Sqlite3Fn._sqlite3_step(stmt);
		Sqlite3Fn._sqlite3_finalize(stmt);
		int changed = Sqlite3Fn._sqlite3_changes(db);
		if (changed == 0)
		{
			Sqlite3Fn._sqlite3_prepare_v2(db,
				"INSERT INTO SE_KvStoreData(key, value, created_at, updated_at) VALUES(?, ?, ?, ?);",
				-1, &stmt, nullptr);

			Sqlite3Fn._sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
			Sqlite3Fn._sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
			Sqlite3Fn._sqlite3_bind_int(stmt, 3, now);
			Sqlite3Fn._sqlite3_bind_int(stmt, 4, now);

			Sqlite3Fn._sqlite3_step(stmt);
			Sqlite3Fn._sqlite3_finalize(stmt);
		}
	}
	inline void Close()
	{
		if (IsOpened())
		{
			auto aranan = db;
			std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
			auto it = std::find_if(g_SqliteConnections.begin(), g_SqliteConnections.end(), [aranan](const SqliteConnectionItem& item) {
				return item.db == aranan;
				});
			if (it != g_SqliteConnections.end())
			{
				it->markAsClosed = true;
			}
		}
		_isopened = false;
	}
};

inline void SQLite_CheckAndCloseConnections()
{
	std::erase_if(g_SqliteConnections, [](const SqliteConnectionItem& connection)
		{
			if (connection.markAsClosed ||!connection.db)
			{
				std::lock_guard<std::mutex> lock(g_SqliteConnectionMutex);
				if(connection.db)
					Sqlite3Fn._sqlite3_close_v2(connection.db);
				return true;
			}
			return false;
		});
}
inline void SqliteWorker()
{
	while (sqliteKeepRun)
	{
		{
			while (!g_SqliteRequestQueues.empty())
			{
				SqliteRequestItem request;
				{
					std::lock_guard<std::mutex> lock(g_SqliteRequestMutex);
					request = g_SqliteRequestQueues.front();
					g_SqliteRequestQueues.pop();
				}
				sqlite3_stmt* stmt;
				if (Sqlite3Fn._sqlite3_prepare_v2(request.db, request.query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {

					SqliteResultItem result;
					result.db = request.db;
					result.callback = request.callback;
					result.userData = request.userData;
					result.isKeyStore = request.isKeyStore;
					result.paramkey = request.paramkey;
					if (request.isKeyStore)
					{
						if (request.keyStoreType == -1) //direct execute
						{
							Sqlite3Fn._sqlite3_prepare_v2(request.db, request.query.c_str(), -1, & stmt, nullptr);
							Sqlite3Fn._sqlite3_step(stmt);
							Sqlite3Fn._sqlite3_finalize(stmt);
						}
						else if (request.keyStoreType == 0) //get
						{
							bool exists = false;
							result.paramvalue = KeyValueStore::PrepareGet(request.db, request.paramkey, exists);
							result.paramExists = exists;
						}
						else if (request.keyStoreType == 1) //set
						{
							KeyValueStore::PrepareSet(request.db, request.paramkey, request.paramvalue);
						}
						else if (request.keyStoreType == 2) //delete
						{
							result.paramcount = KeyValueStore::PrepareDelete(request.db, request.paramkey);
						}
						else if (request.keyStoreType == 3) //deleteall
						{
							result.paramcount =  KeyValueStore::PrepareDeleteAll(request.db);
						}
						else if (request.keyStoreType == 4) //count
						{
							result.paramcount = KeyValueStore::PrepareCount(request.db);
						}
					}
					else {
						if (request.params.size() > 0)
							SqliteConnection::FillParametersFromVec(stmt, request.params);
						while (Sqlite3Fn._sqlite3_step(stmt) == SQLITE_ROW) {
							SqliteRow* row = SqliteRow::Factory();
							row->ReadAllColumns(stmt);
							result.results.push_back(*row);
						}
						Sqlite3Fn._sqlite3_finalize(stmt);
					}

					{
						std::lock_guard<std::mutex> lock(g_SqliteResponseMutex);
						g_SqliteResultQueues.push(result);
					}
				}
			}

		}
		SQLite_CheckAndCloseConnections();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
};
