#pragma once
#include <callbackitem.h>
class JValue;
class MySqlConnection;
class MySqlConnectionConfig;


struct ClientCmdEntry
{
	std::string name;
	asIScriptFunction* callback;
	int order;
};
struct ServerCmdEntry {
	std::string name;
	asIScriptFunction* callback;
	int order;
};

extern std::unordered_map<std::string, std::vector<ClientCmdEntry>> m_ClientCmds;
extern std::unordered_map<std::string, std::vector<ServerCmdEntry>> m_ServerCmds;

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
	CString* BASE64Encode(CString& data);
	CString* BASE64Decode(CString& data);

	RefObject* pDictionary = nullptr;
	MySqlConnection* MySqlConnection_Create(MySqlConnectionConfig& config);
	void* getGlobals();
	void PluginExit();
	void PluginExitIntl(asIScriptModule*);
	bool RegisterClientCmd(CString& name, asIScriptFunction* callback, int ordernum);
	bool UnregisterClientCmd(CString& name, asIScriptFunction* callback);
	bool UnregisterClientCmdsByModule(asIScriptModule* module);
	int TriggerClientCmd(edict_t* edict, const std::string& cmd);
	bool RegisterServerCmd(CString& name, asIScriptFunction* callback, int ordernum);
	bool UnregisterServerCmd(CString& name, asIScriptFunction* callback, int ordernum);
	bool UnregisterServerCmdsByModule(asIScriptModule* module);
	bool ClientCmd(edict_t* edict, CString& command);
	cvar_t* RegisterCvar(CString& name, CString& value, int flags, float flvalue);
	cvar_t* GetCvar(CString& name);
	bool ClientCmdI(size_t index, CString& command);
	int TriggerServerCmd(std::string& name);
	bool ServerCmdExists(std::string name);
	static void ServerCommandHandler();
	CString* SqliteEscape(CString& input);

private:
	void init();
};

struct SvenEnhancerEventItem
{
	asIScriptFunction* callback;
	std::string tag;
};
struct ParsedEvent
{
	std::string name;
	std::string tag;
};
class SvenEnhancerEvent {

public:
	std::unordered_map<std::string, std::vector<SvenEnhancerEventItem>> g_events;
	bool On(CString& name, asIScriptFunction* callback);
	bool Off(CString& name, asIScriptFunction* callback);
	size_t Trigger(CString& name, CallbackItem* item, bool callAll);
	void ClearEventByModule(asIScriptModule* module);
	inline ParsedEvent ParseEvent(const std::string& full)
	{
		ParsedEvent e;
		size_t pos = full.find('#');
		if (pos == std::string::npos)
		{
			e.name = full;
		}
		else
		{
			e.name.assign(full, 0, pos);
			e.tag.assign(full, pos + 1, std::string::npos);
		}
		return e;
	}
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
extern SvenEnhancerAs* g_SE;
extern SvenEnhancerEvent g_SEEvent;