#pragma once
class JValue;
class MySqlConnection;
class MySqlConnectionConfig;

class SvenEnhancerAs : public CASBaseGCObject
{
public:
	SvenEnhancerAs();
	~SvenEnhancerAs();
	JValue* Json_ParseFromFile(CString& input);
	JValue* Json_Parse(CString& input);
	JValue* Json_ParseObject(void* obj, asITypeInfo* typeId);
	JValue* Json_ParseObjectV2(void* obj, int typeId);
	bool MySql_Loaded();
	CString* Interpolate(CString& input, void* dict);
	CString* Version();
	CString* HMAC_SHA256AS(CString& key, CString& data);
	CString* HMAC_SHA1AS(CString& key, CString& data);
	CString* MD5AS(CString& data);
	RefObject* pDictionary = nullptr;
	MySqlConnection* MySqlConnection_Create(MySqlConnectionConfig& config);
	void* getGlobals();
	void PluginExit();

private:
	void init();
};


class SvenEnhancerEnt
{
public:
	std::unordered_map<int, void*> g_EntityData;
	std::unordered_map<int, void*> g_PlayerData;
	SvenEnhancerEnt();
	~SvenEnhancerEnt();
	bool SetPDataInt(void* pThis, int offset, int value, int linuxDiff);
	bool SetPDataUInt(void* pThis, int offset, size_t value, int linuxDiff);
	bool SetPDataBool(void* pThis, int offset, bool value, int linuxDiff);
	bool SetPDataFloat(void* pThis, int offset, float value, int linuxDiff);
	bool SetPDataShort(void* pThis, int offset, short value, int linuxDiff);
	bool SetPDataUShort(void* pThis, int offset, unsigned short value, int linuxDiff);

	int GetPDataInt(void* pThis, int offset, int linuxDiff);
	size_t GetPDataUInt(void* pThis, int offset, int linuxDiff);
	bool GetPDataBool(void* pThis, int offset, int linuxDiff);
	float GetPDataFloat(void* pThis, int offset, int linuxDiff);
	short GetPDataShort(void* pThis, int offset, int linuxDiff);
	unsigned short GetPDataUShort(void* pThis, int offset, int linuxDiff);
	void ClearAllEntityData();
	void ClearPlayerData(int index);
	void* GetDataByEdict(edict_t* edict);
	void* GetDataByEntity(void* entity);
	void* GetData(int index);
};

extern SvenEnhancerEnt g_SEEnt;

