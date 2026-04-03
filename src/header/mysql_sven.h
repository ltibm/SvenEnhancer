#pragma once
#include <mysql.h>
#include <xxhash.h>

#ifdef WIN32
extern HMODULE mysqlHandle;
#else
extern void* mysqlHandle;
#endif
void RegisterMysql();
void RegisterMysqlSymbols();
void RegisterMysqlAngelScript(asIScriptEngine* engine);

struct Mysql
{
    bool loaded;
    decltype(&mysql_init) init;
    decltype(&mysql_real_connect) real_connect;
    decltype(&mysql_close) _close;
    decltype(&mysql_query) query;
    decltype(&mysql_error) error;
    decltype(&mysql_options) options;
    decltype(&mysql_store_result) store_result;
    decltype(&mysql_fetch_row) fetch_row;
    decltype(&mysql_free_result) free_result;
    decltype(&mysql_fetch_fields) fetch_fields;
    decltype(&mysql_affected_rows) affected_rows;
    decltype(&mysql_num_rows) num_rows;
    decltype(&mysql_num_fields) num_fields;
    decltype(&mysql_real_escape_string) escape_string;
    decltype(&mysql_real_escape_string_quote) escape_string_quote;
    decltype(&mysql_ping) ping;
    decltype(&mysql_use_result) use_result;

};
static Mysql MysqlFn;
class MySqlStoreResult;
class MySqlRow;

class MySqlStoreResult : public CASBaseGCObject {
public:
    MySqlStoreResult(MYSQL_RES* res, bool storeAllData);
    ~MySqlStoreResult();
    void Free();
    MySqlRow* FetchRow();
    int GetFieldIndex(std::string name);
    int GetFieldIndexA(CString& name);
    int NumRows();
private:
    friend class MySqlRow;
    bool StoreAllData;
    std::unordered_map<std::string, int> mappedFields;
    MYSQL_RES* res;
};

class MySqlRow : public CASBaseGCObject {
public:
    MySqlRow(MySqlStoreResult* result, MYSQL_ROW row);
    CString* GetString(CString& name);
    CString* GetStringIdx(int index);
    bool IsNull(CString& name);
    bool IsNullIdx(int index);
    bool GetBool(CString& name);
    bool GetBoolIdx(int index);
    int GetInt(CString& name);
    int GetIntIdx(int index);
    unsigned int GetUInt(CString& name);
    unsigned int GetUIntIdx(int index);
    double GetDouble(CString& name);
    double GetDoubleIdx(int index);
private:
    MYSQL_ROW row;
    MySqlStoreResult* result;
};

class MySqlConnection : public CASBaseGCObject
{
public:
    MySqlConnection();
    ~MySqlConnection();
    static bool Available();
    bool Init(MySqlConnectionConfig* config);
    bool Open();
    bool IsOpened();
	void Close();
    bool Query(CString* query);
    int AffectedRows();
    CString* GetError();
    MySqlStoreResult* StoreResult(bool storeAllData);
    MySqlConnectionConfig* config = nullptr;
    CString* Escape(CString& input);
    CString* EscapeQuote(CString& input, unsigned char quote);
    MYSQL* connection = nullptr;
private:
    bool _isopened = false;
};


class MySqlConnectionConfig
{
public:
    MySqlConnectionConfig();
    MySqlConnectionConfig(const MySqlConnectionConfig& o);
    ~MySqlConnectionConfig();
    std::string host = "";
    std::string username = "";
    std::string password = "";
    std::string database = "";
    int port = 3306;
    CString* GetHost();
    void SetHost(CString& v);
    CString* GetUsername();
    void SetUsername(CString& v);
    CString* GetPassword();
    void SetPassword(CString& v);
    CString* GetDatabase();
    void SetDatabase(CString& v);
    uint64_t GetHash() const;
    static MySqlConnectionConfig* Factory(MySqlConnectionConfig* mem);
    static MySqlConnectionConfig* FactoryCopy(MySqlConnectionConfig* mem, const MySqlConnectionConfig& o);
    static void Destruct(MySqlConnectionConfig* item);
};

