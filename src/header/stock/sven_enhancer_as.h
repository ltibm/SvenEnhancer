#pragma once
#include <callbackitem.h>
#include <tvalue.h>
#define MAKE_EVENTKEY(type, inner) (((unsigned long long)(type) << 32) | (unsigned int)(inner)) 
#define EVENT_MESSAGE 1
#define EVENT_GETGAMEDESCRIPTION 2
#define EVENT_CLIENTUSERINFOCHANGED 3
#define EVENT_KEYVALUE 4
#define EVENT_PLAYERQUERYCVAR 5
#define EVENT_PLAYERQUERYCVAR2 6


class JValue;
class MySqlConnection;
class MySqlConnectionConfig;


struct ClientCmdEntry
{
	std::string name;
	SEFunction* callback;
	int order = 0;
};
struct ServerCmdEntry {
	std::string name;
	SEFunction* callback;
	int order = 0;
};

struct VoiceState {
	int teamId = 0;
	bool isMuted = false;
	bool isAdmin = false;
	int level = 0;
};

struct AdminState {
	bool isAdmin = false;
	bool isOwner = false;
};
extern std::unordered_map<std::string, AdminState> g_AdminMap;
extern std::unordered_map<std::string, std::vector<ClientCmdEntry>> m_ClientCmds;
extern std::unordered_map<std::string, std::vector<ServerCmdEntry>> m_ServerCmds;
inline const char* GetSteamID(edict_t* ent)
{
	if (!ent || ent->free)
		return nullptr;

	return (*g_engfuncs.pfnGetPlayerAuthId)(ent);
}
inline AdminState GetAdminState(const std::string& steamid)
{
	auto it = g_AdminMap.find(steamid);
	if (it != g_AdminMap.end())
		return it->second;
	return { false, false };
}
inline AdminState GetAdminState(edict_t* ent)
{
	const char* steamid = GetSteamID(ent);
	if (!steamid)
		return { false, false };
	auto it = g_AdminMap.find(steamid);
	if (it != g_AdminMap.end())
		return it->second;
	return { false, false };
}
inline AdminState GetAdminState(int index)
{
	edict_t* ent = INDEXENT(index);
	if (!ent || ent->free)
		return { false, false };
	const char* steamid = GetSteamID(ent);
	if (!steamid)
		return { false, false };
	auto it = g_AdminMap.find(steamid);
	if (it != g_AdminMap.end())
		return it->second;
	return { false, false };
}
class SvenEnhancerAs : public CASBaseGCObject
{
public:
	SvenEnhancerAs();
	~SvenEnhancerAs();
	JValue* Json_ParseFromFile(CString& input);
	JValue* Json_Parse(CString& input);
	TValue* Toml_ParseFromFile(CString& input);
	TValue* Toml_Parse(CString& input);
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
	CString* InfoKeyValue(CString& info, CString& key);

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
	VoiceState* GetVoiceState(int index);
	VoiceState* GetVoiceStateE(edict_t* edict);
	VoiceState g_VoiceState[33];
	bool voice_AdminOnly = false;
	bool voice_Disabled = false;
	int voice_MinLevel = 0;
	inline void QueryClientCvarValueI(int index, CString& cvarName)
	{
		
		edict_t* edict = INDEXENT(index);
		QueryClientCvarValue(edict, cvarName);
	}
	inline void QueryClientCvarValue(const edict_t* player, CString& cvarName)
	{
		if (!player || player->free || !player->pvPrivateData || !(player->v.flags & FL_CLIENT))
			return;
		QUERY_CLIENT_CVAR_VALUE(player, cvarName.c_str());
	}
	inline void QueryClientCvarValue2I(int index, CString& cvarName, int requestId)
	{
		edict_t* edict = INDEXENT(index);
		QueryClientCvarValue2(edict, cvarName, requestId);
	}
	inline void QueryClientCvarValue2(const edict_t* player, CString& cvarName, int requestId)
	{
		if (!player || player->free || !player->pvPrivateData || !(player->v.flags & FL_CLIENT))
			return;
		QUERY_CLIENT_CVAR_VALUE2(player, cvarName.c_str(), requestId);
	}

private:
	void init();
};

struct SvenEnhancerEventItem
{
	SEFunction* callback;
	std::string tag;
	//asIScriptModule* module;
};
struct ParsedEvent
{
	std::string name;
	std::string tag;
};

class SvenEnhancerFile {
public:
	inline bool Exists(CString& path)
	{
		namespace fs = std::filesystem;
		std::filesystem::path _path(path.c_str());
		std::filesystem::path rootPath = "./svencoop";
		std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
		return fs::exists(full);
	}
	inline bool IsDirectory(CString& path)
	{
		namespace fs = std::filesystem;
		std::filesystem::path _path(path.c_str());
		std::filesystem::path rootPath = "./svencoop";
		std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
		return fs::exists(full) && fs::is_directory(full);
	}
	CScriptArray* GetFiles(CString& path, bool includeDirectory, CString& filter, bool recursive = false);

};
struct DynamicArg;
class SvenEnhancerEvent {

public:
	std::unordered_map<std::string, std::vector<SvenEnhancerEventItem>> g_events;
	std::unordered_map<unsigned long long, std::vector<SvenEnhancerEventItem>> g_gameEvents;
	bool On(CString& name, asIScriptFunction* callback);
	bool OnGameEvent(CString& key, void* obj, int typeId);
	bool Off(CString& name, asIScriptFunction* callback);
	bool OffGameEvent(CString& key, void* obj, int typeId);
	size_t Trigger(CString& name, CallbackItem* item, bool callAll);
	size_t TriggerGameEvent(uint64_t key, CallbackItem* item, const std::vector<DynamicArg>& args = {}, bool callAll = false);
	void ClearEventByModule(asIScriptModule* module);
	inline ParsedEvent ParseEvent(const std::string& full)
	{
		ParsedEvent e;
		size_t pos = full.rfind('#');
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
extern SvenEnhancerFile g_SEFile;


void LoadAdmins();