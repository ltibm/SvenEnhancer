#include <svenenhancer.h>

#include <mysql_sven.h>
#include <mysql_connection_pool.h>
#ifndef WIN32
#include <filesystem>
#endif

bool MySqlLoaded;
#ifdef WIN32
HMODULE  mysqlHandle;
#else
void* mysqlHandle;
#endif

void RegisterMysql() {
	namespace fs = std::filesystem;

	std::string pluginPath = GET_PLUGIN_PATH(PLID);
	pluginPath = pluginPath.substr(0, pluginPath.find_last_of("/\\"));

#ifdef _WIN32 
	std::string libPath = pluginPath + "\\se_libs\\libmysql.dll";
#else
	std::string libPath = pluginPath + "/se_libs/libmysqlclient.so";
#endif
	fs::path p = libPath;
	if (fs::exists(p) && fs::is_regular_file(p))
	{
#ifdef _WIN32
		HMODULE lib = GetModuleHandleA(p.string().c_str());
		if (!lib)
			lib = LoadLibraryA(p.string().c_str());
#else
		void* lib = dlopen(p.string().c_str(), RTLD_NOLOAD);
		if (!lib)
			lib = dlopen(libPath.c_str(), RTLD_LAZY);
#endif
		if (!lib)
		{
			SE_PRINT("Failed to load MySql library");
		}
		else {
			mysqlHandle = lib;
			RegisterMysqlSymbols();
		}
	}
	else {
		SE_PRINT("MySql Library Not Found");
	}
}

void RegisterMysqlSymbols()
{
	MysqlFn.init = GetSymbol<decltype(&mysql_init)>(mysqlHandle, "mysql_init");
	MysqlFn.real_connect = GetSymbol<decltype(&mysql_real_connect)>(mysqlHandle, "mysql_real_connect");
	MysqlFn._close = GetSymbol<decltype(&mysql_close)>(mysqlHandle, "mysql_close");
	MysqlFn.query = GetSymbol<decltype(&mysql_query)>(mysqlHandle, "mysql_query");
	MysqlFn.error = GetSymbol<decltype(&mysql_error)>(mysqlHandle, "mysql_error");
	MysqlFn.options = GetSymbol<decltype(&mysql_options)>(mysqlHandle, "mysql_options");
	MysqlFn.store_result = GetSymbol<decltype(&mysql_store_result)>(mysqlHandle, "mysql_store_result");
	MysqlFn.fetch_row = GetSymbol<decltype(&mysql_fetch_row)>(mysqlHandle, "mysql_fetch_row");
	MysqlFn.free_result = GetSymbol<decltype(&mysql_free_result)>(mysqlHandle, "mysql_free_result");
	MysqlFn.fetch_fields = GetSymbol<decltype(&mysql_fetch_fields)>(mysqlHandle, "mysql_fetch_fields");
	MysqlFn.affected_rows = GetSymbol<decltype(&mysql_affected_rows)>(mysqlHandle, "mysql_affected_rows");
	MysqlFn.num_rows = GetSymbol<decltype(&mysql_num_rows)>(mysqlHandle, "mysql_num_rows");
	MysqlFn.num_fields = GetSymbol<decltype(&mysql_num_fields)>(mysqlHandle, "mysql_num_fields");
	MysqlFn.escape_string = GetSymbol<decltype(&mysql_real_escape_string)>(mysqlHandle, "mysql_real_escape_string");
	MysqlFn.escape_string_quote = GetSymbol<decltype(&mysql_real_escape_string_quote)>(mysqlHandle, "mysql_real_escape_string_quote");
	MysqlFn.ping = GetSymbol<decltype(&mysql_ping)>(mysqlHandle, "mysql_ping");
	MysqlFn.use_result = GetSymbol<decltype(&mysql_use_result)>(mysqlHandle, "mysql_use_result");
	MysqlFn.loaded = MysqlFn.init && MysqlFn.real_connect && MysqlFn._close && MysqlFn.query && MysqlFn.error && MysqlFn.ping && MysqlFn.use_result;
}

void RegisterMysqlAngelScript(asIScriptEngine* engine) {
	int r = 0;
	asSFuncPtr reg;

	//r = RegisterObject<MySqlConnectionConfig>("MySqlConnectionConfig", engine, asOBJ_VALUE | asOBJ_APP_CLASS);
	r = engine->RegisterObjectType("MySqlConnectionConfig", sizeof(MySqlConnectionConfig), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MySqlConnectionConfig", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(MySqlConnectionConfig::Factory), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MySqlConnectionConfig", asBEHAVE_CONSTRUCT, "void f(const MySqlConnectionConfig &in)", asFUNCTION(MySqlConnectionConfig::FactoryCopy), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	//r = engine->RegisterObjectBehaviour("MySqlConnectionConfig", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(MySqlConnectionConfig::Factory), asCALL_CDECL_OBJLAST); assert(r >= 0);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "string& get_Host() const", asMETHOD(MySqlConnectionConfig, GetHost), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "void set_Host(string& in v)", asMETHOD(MySqlConnectionConfig, SetHost), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "string& get_Username() const", asMETHOD(MySqlConnectionConfig, GetUsername), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "void set_Username(string& in v)", asMETHOD(MySqlConnectionConfig, SetUsername), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "string& get_Password() const", asMETHOD(MySqlConnectionConfig, GetPassword), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "void set_Password(string& in v)", asMETHOD(MySqlConnectionConfig, SetPassword), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "string& get_Database() const", asMETHOD(MySqlConnectionConfig, GetDatabase), asCALL_THISCALL);
	engine->RegisterObjectMethod("MySqlConnectionConfig", "void set_Database(string& in v)", asMETHOD(MySqlConnectionConfig, SetDatabase), asCALL_THISCALL);
	//engine->RegisterObjectProperty("MySqlConnectionConfig", "string Host", offsetof(MySqlConnectionConfig, host));
	//engine->RegisterObjectProperty("MySqlConnectionConfig", "string Username", offsetof(MySqlConnectionConfig, username));
	//engine->RegisterObjectProperty("MySqlConnectionConfig", "string Password", offsetof(MySqlConnectionConfig, password));
	//engine->RegisterObjectProperty("MySqlConnectionConfig", "string Database", offsetof(MySqlConnectionConfig, database));
	engine->RegisterObjectProperty("MySqlConnectionConfig", "int Port", offsetof(MySqlConnectionConfig, port));

	r = RegisterObject<MySqlRow>("MySqlRow", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("MySqlRow", "string& GetString(string& in name) const", asMETHOD(MySqlRow, GetString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "string& GetString(int index) const", asMETHOD(MySqlRow, GetStringIdx), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MySqlRow", "bool GetBool(string& in name) const", asMETHOD(MySqlRow, GetBool), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "bool GetBool(int index) const", asMETHOD(MySqlRow, GetBoolIdx), asCALL_THISCALL); assert(r >= 0);



	r = engine->RegisterObjectMethod("MySqlRow", "int GetInt(string& in name) const", asMETHOD(MySqlRow, GetInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "int GetInt(int index) const", asMETHOD(MySqlRow, GetIntIdx), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MySqlRow", "uint GetUInt(string& in name) const", asMETHOD(MySqlRow, GetUInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "uint GetUInt(int index) const", asMETHOD(MySqlRow, GetUIntIdx), asCALL_THISCALL); assert(r >= 0);



	r = engine->RegisterObjectMethod("MySqlRow", "double GetDouble(string& in name) const", asMETHOD(MySqlRow, GetDouble), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "double GetDouble(int index) const", asMETHOD(MySqlRow, GetDoubleIdx), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MySqlRow", "bool IsNull(string& in name) const", asMETHOD(MySqlRow, IsNull), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlRow", "bool IsNull(int index) const", asMETHOD(MySqlRow, IsNullIdx), asCALL_THISCALL); assert(r >= 0);



	r = RegisterObject<MySqlStoreResult>("MySqlStoreResult", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("MySqlStoreResult", "void Free()", asMETHOD(MySqlStoreResult, Free), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlStoreResult", "MySqlRow@ FetchRow()", asMETHOD(MySqlStoreResult, FetchRow), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlStoreResult", "int GetFieldIndex(string&in name)", asMETHOD(MySqlStoreResult, GetFieldIndexA), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlStoreResult", "int NumRows()", asMETHOD(MySqlStoreResult, NumRows), asCALL_THISCALL); assert(r >= 0);


	r = RegisterObject<MySqlConnection>("MySqlConnection", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("MySqlConnection", "bool Open() const", asMETHOD(MySqlConnection, Open), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "bool IsOpened() const", asMETHOD(MySqlConnection, IsOpened), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "string& GetError() const", asMETHOD(MySqlConnection, GetError), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "void Close()", asMETHOD(MySqlConnection, Close), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "bool Query(string& in query) const", asMETHOD(MySqlConnection, Query), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "int AffectedRows() const", asMETHOD(MySqlConnection, AffectedRows), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "MySqlStoreResult@ StoreResult(bool storeAllData = false)", asMETHOD(MySqlConnection, StoreResult), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "string& Escape(string&in input) const", asMETHOD(MySqlConnection, Escape), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("MySqlConnection", "string& EscapeQuote(string&in input, char quote) const", asMETHOD(MySqlConnection, EscapeQuote), asCALL_THISCALL); assert(r >= 0);



}

MySqlConnection::MySqlConnection() {
	;


}
MySqlConnection::~MySqlConnection() {
	if (this->IsOpened())
	{
		this->Close();
	}

}
bool MySqlConnection::Init(MySqlConnectionConfig* config)
{
	if (!MysqlFn.loaded)
		return false;
	this->config = config;
	this->connection = MysqlFn.init(nullptr);
	if (this->connection)
		return true;
	return false;
}
bool MySqlConnection::Open()
{
	if (this->IsOpened())
		return false;
	if (this->connection)
	{
		auto hash = this->config->GetHash();



		unsigned int ssl_mode = SSL_MODE_DISABLED;
		MysqlFn.options(this->connection, MYSQL_OPT_SSL_MODE, &ssl_mode);
		if (MysqlFn.real_connect(this->connection, this->config->host.c_str(), this->config->username.c_str(), this->config->password.c_str(), this->config->database.c_str(), this->config->port, nullptr, 0))
		{
			return true;
		}
	}
	return false;
}
bool MySqlConnection::IsOpened()
{
	return !this->connection || _isopened;
}

void MySqlConnection::Close()
{
	if (this->IsOpened())
	{
		MysqlFn._close(this->connection);
	}
}
bool MySqlConnection::Query(CString* query)
{
	if (!query || !this->connection)
		return 0;
	auto result = MysqlFn.query(this->connection, query->c_str());
	return result == 0;
}
int MySqlConnection::AffectedRows()
{
	if (!this->connection)
		return 0;
	return MysqlFn.affected_rows(this->connection);
}
bool MySqlConnection::Available()
{
	return MysqlFn.loaded;
}
CString* MySqlConnection::GetError()
{
	if (!this->connection)
		return nullptr;
	auto error = MysqlFn.error(this->connection);
	if (error)
	{
		CString* str = new CString();
		str->assign(error, strlen(error));
		return str;
	}
	return nullptr;
}
CString* MySqlConnection::Escape(CString& input)
{
	if (!this->connection)
		return nullptr;
	char escaped[1024];
	unsigned long len = MysqlFn.escape_string(
		this->connection,
		escaped,
		input.c_str(),
		strlen(input.c_str())
	);
	CString* esc = new CString();
	esc->assign(escaped, len);
	return esc;
}
CString* MySqlConnection::EscapeQuote(CString& input, unsigned char quote)
{
	if (!this->connection)
		return nullptr;
	char escaped[1024];
	unsigned long len = MysqlFn.escape_string_quote(
		this->connection,
		escaped,
		input.c_str(),
		strlen(input.c_str()),
		quote
	);
	CString* esc = new CString();
	esc->assign(escaped, len);
	return esc;
}
MySqlStoreResult* MySqlConnection::StoreResult(bool storeAllData)
{
	auto result = storeAllData ? MysqlFn.store_result(this->connection) : MysqlFn.use_result(this->connection);
	if (!result)
		return nullptr;
	MySqlStoreResult* sr = new MySqlStoreResult(result, storeAllData);
	return sr;

}

MySqlConnectionConfig::MySqlConnectionConfig() {

}
MySqlConnectionConfig::~MySqlConnectionConfig() {


}
CString* MySqlConnectionConfig::GetHost() {
	CString* str = new CString();
	str->assign(this->host.c_str(), this->host.length());
	return str;
}
void MySqlConnectionConfig::SetHost(CString& v)
{
	this->host = v.c_str();
}
CString* MySqlConnectionConfig::GetUsername() {
	CString* str = CreateString(this->username.c_str());
	return str;
}
void MySqlConnectionConfig::SetUsername(CString& v)
{
	this->username = v.c_str();
}
CString* MySqlConnectionConfig::GetPassword() {
	CString* str = CreateString(this->password.c_str());
	return str;
}
void MySqlConnectionConfig::SetPassword(CString& v)
{
	this->password = v.c_str();
}
CString* MySqlConnectionConfig::GetDatabase() {
	CString* str = CreateString(this->database.c_str());
	return str;
}
void MySqlConnectionConfig::SetDatabase(CString& v)
{
	this->database = v.c_str();
}

uint64_t MySqlConnectionConfig::GetHash() const
{
	std::string data;
	data.reserve(
		host.size() +
		username.size() +
		password.size() +
		database.size() +
		32
	);
	data += host + '|' + username + '|' + password + '|' + database + '|' + std::to_string(port);
	return XXH3_64bits(data.data(), data.size());
}

MySqlConnectionConfig::MySqlConnectionConfig(const MySqlConnectionConfig& o) {
	this->username = o.username;
	this->password = o.password;
	this->database = o.database;
	this->host = o.host;
	this->port = o.port;
}

MySqlConnectionConfig* MySqlConnectionConfig::Factory(MySqlConnectionConfig* mem) {
	MySqlConnectionConfig* obj = new MySqlConnectionConfig();
	//asIScriptEngine* engine = GetASEngine();
	//asITypeInfo* type = engine->GetTypeInfoByName("MySqlConnectionConfig");
	//engine->NotifyGarbageCollectorOfNewObject(obj, type);
	//return obj;
	new(mem) MySqlConnectionConfig();
	return obj;
}
MySqlConnectionConfig* MySqlConnectionConfig::FactoryCopy(MySqlConnectionConfig* mem, const MySqlConnectionConfig& o) {
	new(mem) MySqlConnectionConfig(o);
	return nullptr;
}
void MySqlConnectionConfig::Destruct(MySqlConnectionConfig* item) {
	item->~MySqlConnectionConfig();
}


MySqlStoreResult::MySqlStoreResult(MYSQL_RES* res, bool storeAllData)
{
	this->res = res;
	this->StoreAllData = storeAllData;
	if (res)
	{
		std::unordered_map<std::string, int> colIndex;
		auto fields = MysqlFn.fetch_fields(res);
		unsigned int numFields = MysqlFn.num_fields(res);
		for (unsigned int i = 0; i < numFields; i++)
		{
			colIndex[fields[i].name] = i;
		}
		mappedFields = colIndex;
	}
}
MySqlStoreResult::~MySqlStoreResult()
{
	this->Free();
}
void MySqlStoreResult::Free()
{
	if (this->res)
	{
		MysqlFn.free_result(this->res);
		this->res = nullptr;
	}
}
int MySqlStoreResult::GetFieldIndex(std::string name)
{
	auto it = this->mappedFields.find(name);
	if (it == this->mappedFields.end())
	{
		return -1;
	}
	return it->second;
}
int MySqlStoreResult::GetFieldIndexA(CString& name)
{
	std::string _name = name.c_str();
	return this->GetFieldIndex(_name);
}

int MySqlStoreResult::NumRows()
{
	if (!this->res || !this->StoreAllData)
		return -1;
	return MysqlFn.num_rows(this->res);
}

MySqlRow* MySqlStoreResult::FetchRow()
{
	if (!this->res)
		return nullptr;
	auto nextRow = MysqlFn.fetch_row(this->res);
	if (nextRow)
	{
		return new MySqlRow(this, nextRow);
	}
	return nullptr;
}


MySqlRow::MySqlRow(MySqlStoreResult* result, MYSQL_ROW row)
{
	this->result = result;
	this->row = row;
}
CString* MySqlRow::GetString(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetStringIdx(idx);
}
CString* MySqlRow::GetStringIdx(int index)
{
	CString* str = CreateString("");
	if (index >= 0 && row[index] != nullptr)
		str->assign(row[index], strlen(row[index]));
	return str;
}
int MySqlRow::GetInt(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetIntIdx(idx);
}
int MySqlRow::GetIntIdx(int index)
{
	if (index >= 0 && row[index] != nullptr)
		return std::stoi(row[index]);
	return 0;
}
unsigned int MySqlRow::GetUInt(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetUIntIdx(idx);
}
unsigned int MySqlRow::GetUIntIdx(int index)
{
	if (index >= 0 && row[index] != nullptr)
		return static_cast<unsigned int>(std::stoul(row[0]));
	return 0;
}
double MySqlRow::GetDouble(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetDoubleIdx(idx);
}
double MySqlRow::GetDoubleIdx(int index)
{
	if (index >= 0 && row[index] != nullptr)
		return std::stod(row[index]);
	return 0;
}
bool MySqlRow::GetBool(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetBoolIdx(idx);
}
bool MySqlRow::GetBoolIdx(int index)
{
	if (index >= 0 && row[index] != nullptr)
		return std::stoi(row[index]) != 0;
	return false;
}
bool MySqlRow::IsNull(CString& name)
{
	int idx = this->result->GetFieldIndex(name.c_str());
	return GetBoolIdx(idx);
}
bool MySqlRow::IsNullIdx(int index)
{
	if (index >= 0 && row[index] != nullptr)
		return false;
	return true;
}
