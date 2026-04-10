#include <svenenhancer.h>
#include <mysql_sven.h>
#include <callbackitem.h>

std::unordered_map<std::string, std::vector<ClientCmdEntry>> m_ClientCmds;
std::unordered_map<std::string, std::vector<ServerCmdEntry>> m_ServerCmds;

SvenEnhancerAs::SvenEnhancerAs() {
	init();
}

void SvenEnhancerAs::init() {
	auto engine = GetASEngine();
	pDictionary = new RefObject();
	asITypeInfo* dictInfo = engine->GetTypeInfoByDecl("dictionary");
	pDictionary->type = dictInfo;
	pDictionary->data = engine->CreateScriptObject(dictInfo);
	engine->AddRefScriptObject(pDictionary->data, dictInfo);
	engine->NotifyGarbageCollectorOfNewObject(pDictionary->data, dictInfo);
}
SvenEnhancerAs::~SvenEnhancerAs() {
	if (pDictionary && pDictionary->data)
	{
		auto engine = GetASEngine();
		engine->ReleaseScriptObject(pDictionary->data, pDictionary->type);
	}
	delete pDictionary;
}
void* SvenEnhancerAs::getGlobals()
{
	//this->AddRef();
	return pDictionary->data;
}
JValue* SvenEnhancerAs::Json_ParseFromFile(CString& input) {
	//this->AddRef();
	std::string jsonString = input.c_str();
	if (jsonString.ends_with(".as"))
	{
		return nullptr;
	}
	std::filesystem::path _path(jsonString);
	std::filesystem::path rootPath = "./svencoop";
	std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
	try {
		auto jv = new JValue();
		std::ifstream fStream(full);
		jv->json = nlohmann::json::parse(fStream, nullptr, true, true);
		return jv;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		printf("error when parsing: %s\r\n", e.what());
	}
	return nullptr;

}

JValue* SvenEnhancerAs::Json_Parse(CString& input) {
	//this->AddRef();
	std::string jsonString = input.c_str();
	try {
		auto jv = new JValue();
		jv->json = nlohmann::json::parse(jsonString, nullptr, true, true);
		return jv;

	}
	catch (const nlohmann::json::parse_error& e)
	{
		printf("error when parsing: %s\r\n", e.what());
	}
	return nullptr;

}
JValue* SvenEnhancerAs::Json_ParseObject(void* obj, asITypeInfo* type)
{
	//this->AddRef();
	if (!obj || !type)
		return nullptr;
	JValue* jv = new JValue();
	jv->FillFromObject(obj, type);
	return jv;
}
JValue* SvenEnhancerAs::Json_ParseObjectV2(void* obj, int typeId)
{
	//this->AddRef();
	//Prevent crash for @class
	if (typeId & asTYPEID_OBJHANDLE)
		obj = *(void**)obj;

	auto type = GetASEngine()->GetTypeInfoById(typeId);
	if (!obj || !type)
		return nullptr;
	JValue* jv = new JValue();
	jv->FillFromObject(obj, type);
	return jv;
}
MySqlConnection* SvenEnhancerAs::MySqlConnection_Create(MySqlConnectionConfig& config)
{
	//this->AddRef();
	// libmysql is requierd for this method
	if (!MySqlConnection::Available())
		return nullptr;
	MySqlConnection* connection = new MySqlConnection();
	if (!connection->Init(&config)) {
		return nullptr;
	}
	return connection;
}
bool SvenEnhancerAs::MySql_Loaded()
{
	return MysqlFn.loaded;
}
CString* SvenEnhancerAs::Version() {
	CString* res = new CString();;
	res->assign(SE_VERSION, strlen(SE_VERSION));
	return res;
}
CString* SvenEnhancerAs::HMAC_SHA1AS(CString& key, CString& data)
{
	auto res = RestUtils::HMAC_SHA1(key.c_str(), data.c_str());
	return CreateString(res.c_str());
}
CString* SvenEnhancerAs::BASE64Encode(CString& data)
{
	auto res = RestUtils::Base64Encode(data.c_str());
	return CreateString(res.c_str());
}
CString* SvenEnhancerAs::BASE64Decode(CString& data)
{
	auto res = RestUtils::Base64Decode(data.c_str());
	return CreateString(res.c_str());
}
CString* SvenEnhancerAs::HMAC_SHA256AS(CString& key, CString& data)
{
	auto res = RestUtils::HMAC_SHA256(key.c_str(), data.c_str());
	return CreateString(res.c_str());
}
CString* SvenEnhancerAs::MD5AS(CString& data)
{
	auto res = RestUtils::ToMD5(data.c_str());
	return CreateString(res.c_str());
}
CString* SvenEnhancerAs::Interpolate(CString& input, void* dict)
{
	if (!dict)
		return &input;
	std::locale old;
	std::string text = input.c_str();
	std::regex double_braces(R"(\{\{([^{}]+)\}\})", std::regex_constants::extended);
	text = std::regex_replace(text, double_braces, R"({$1})");
	std::regex single_braces(R"(\{([^{}]+)\})", std::regex_constants::extended);
	std::smatch match;
	CDictHelper* helper = new CDictHelper(dict);
	while (std::regex_search(text, match, single_braces))
	{
		std::string key = match[1].str();
		auto splitted = split(key, ":");
		if (splitted.size() == 0)
			continue;
		std::string fullMatch = match[0].str();
		std::string fmt;
		if (splitted.size() > 1)
			fmt = splitted[1];
		CString* k = new CString();;
		k->assign(splitted[0].c_str(), splitted[0].length());
		std::string replacement = helper->getStdString(*k, fmt);
		text = match.prefix().str() + replacement + match.suffix().str();
		k->dtor();
	}
	CString* res = new CString();;
	res->assign(text.c_str(), text.length());
	//std::locale::global(old);
	return  res;
}
void SvenEnhancerAs::PluginExit()
{
	//No Need
}

void SvenEnhancerAs::PluginExitIntl(asIScriptModule* module)
{
	//auto module = GetModulePtr(ASEXT_GetServerManager());
	if (module)
	{
		deleteModuleData(module);

	}
}

bool SvenEnhancerAs::RegisterClientCmd(CString& name, asIScriptFunction* callback, int ordernum)
{
	std::string _name(name.c_str());
	if (_name.empty() || !callback)
		return false;

	auto& list = m_ClientCmds[_name];
	callback->AddRef();
	list.push_back({ _name, callback, ordernum });
	std::sort(list.begin(), list.end(),
		[](const ClientCmdEntry& a, const ClientCmdEntry& b)
		{
			return a.order < b.order;
		});
	return true;
}

bool SvenEnhancerAs::UnregisterClientCmd(CString& name, asIScriptFunction* callback)
{
	std::string _name(name.c_str());
	if (_name.empty() || !callback)
		return false;
	bool removed = false;
	for (auto& [name, list] : m_ClientCmds)
	{
		auto it = std::remove_if(list.begin(), list.end(),
			[&](const ClientCmdEntry& e)
			{
				return e.callback == callback;
			});
		if (it != list.end())
		{
			for (auto i = it; i != list.end(); ++i)
			{
				i->callback->Release();
				removed = true;
			}

			list.erase(it, list.end());
		}
	}
	return removed;
}

bool SvenEnhancerAs::UnregisterClientCmdsByModule(asIScriptModule* module)
{
	if (!module)
		return false;
	bool removed = false;
	for (auto& [name, list] : m_ClientCmds)
	{
		auto it = std::remove_if(list.begin(), list.end(),
			[&](const ClientCmdEntry& e)
			{
				return e.callback && e.callback->GetModule() == module;
			});

		if (it != list.end())
		{
			for (auto i = it; i != list.end(); ++i)
			{
				if (i->callback)
					i->callback->Release();

				removed = true;
			}
			list.erase(it, list.end());
		}
	}
	return removed;
}

int SvenEnhancerAs::TriggerClientCmd(edict_t* edict, const std::string& cmd)
{
	if (!edict)
		return 0;
	int lastr = 0;
	auto engine = GetASEngine();

	std::string key = cmd;

	auto it = m_ClientCmds.find(cmd);
	if (it == m_ClientCmds.end())
		return lastr;

	auto& list = it->second;
	auto snapshot = list;
	CallbackItem item;
	item.AddRef();

	for (auto& entry : snapshot)
	{
		if (!entry.callback)
			continue;

		//item.StopCall = false;
		//item.ReturnCode = 0;
		asIScriptContext* ctx = engine->RequestContext();
		if (!ctx)
			continue;
		ctx->Prepare(entry.callback);
		ctx->SetArgAddress(0, edict);
		ctx->SetArgAddress(1, &item);
		int r = ctx->Execute();
		lastr = ctx->GetReturnDWord();
		engine->ReturnContext(ctx);
		if (item.StopCall)
			break;
	}
	return lastr;
}

bool SvenEnhancerAs::RegisterServerCmd(CString& name, asIScriptFunction* callback, int ordernum)
{
	std::string _name(name.c_str());
	if (_name.empty() || !callback)
		return false;
	bool exists = ServerCmdExists(_name);
	auto& list = m_ServerCmds[_name];
	callback->AddRef();
	list.push_back({ _name, callback, ordernum });
	std::sort(list.begin(), list.end(),
		[](const ServerCmdEntry& a, const ServerCmdEntry& b)
		{
			return a.order < b.order;
		});
	//Check handler is exists
	if (!exists)
	{
		//Register handler
		REG_SVR_COMMAND((char*)name.c_str(), SvenEnhancerAs::ServerCommandHandler);
	}
	return true;
}

bool SvenEnhancerAs::UnregisterServerCmd(CString& name, asIScriptFunction* callback, int ordernum)
{
	std::string _name(name.c_str());
	if (_name.empty() || !callback)
		return false;
	bool removed = false;
	for (auto& [name, list] : m_ServerCmds)
	{
		auto it = std::remove_if(list.begin(), list.end(),
			[&](const ServerCmdEntry& e)
			{
				return e.callback == callback;
			});
		if (it != list.end())
		{
			for (auto i = it; i != list.end(); ++i)
			{
				i->callback->Release();
				removed = true;
			}

			list.erase(it, list.end());
		}
	}
	return removed;
}

bool SvenEnhancerAs::UnregisterServerCmdsByModule(asIScriptModule* module)
{
	if (!module)
		return false;
	bool removed = false;
	for (auto& [name, list] : m_ServerCmds)
	{
		auto it = std::remove_if(list.begin(), list.end(),
			[&](const ServerCmdEntry& e)
			{
				return e.callback && e.callback->GetModule() == module;
			});

		if (it != list.end())
		{
			for (auto i = it; i != list.end(); ++i)
			{
				if (i->callback)
					i->callback->Release();

				removed = true;
			}
			list.erase(it, list.end());
		}
	}
	return removed;
}

int SvenEnhancerAs::TriggerServerCmd(std::string& name)
{
	int lastr = 0;
	auto engine = GetASEngine();

	std::string key = name;

	auto it = m_ServerCmds.find(name);
	if (it == m_ServerCmds.end())
		return lastr;

	auto& list = it->second;
	auto snapshot = list;
	CallbackItem item;
	item.AddRef();
	for (auto& entry : snapshot)
	{
		if (!entry.callback)
			continue;
		//item.StopCall = false;
		//item.ReturnCode = 0;
		asIScriptContext* ctx = engine->RequestContext();
		if (!ctx)
			continue;
		ctx->Prepare(entry.callback);
		ctx->SetArgAddress(0, &item);
		int r = ctx->Execute();
		lastr = ctx->GetReturnDWord();
		engine->ReturnContext(ctx);
		if (item.StopCall)
			break;
	}
	return lastr;
}

bool SvenEnhancerAs::ServerCmdExists(std::string name)
{
	return m_ServerCmds.contains(name);
}

void SvenEnhancerAs::ServerCommandHandler()
{
	std::string _cmd = CMD_ARGV(0);
	g_SE->TriggerServerCmd(_cmd);
}

bool SvenEnhancerAs::ClientCmd(edict_t* edict, CString& command)
{
	if (!edict || command.empty())
		return false;
	std::string cmd(command.c_str());
	std::string fixed = cmd;
	while (!fixed.empty() && fixed.back() == ' ')
		fixed.pop_back();
	char last = fixed.back();
	if (last != '\n' && last != ';')
	{
		fixed += '\n';
	}
	CLIENT_COMMAND(edict, (char*)cmd.c_str());
	return true;
}
cvar_t* SvenEnhancerAs::RegisterCvar(CString& name, CString& value, int flags, float flvalue)
{
	if (name.empty())
		return nullptr;
	cvar_t* cvar = new cvar_t();
	cvar->name = (char*)name.c_str();
	cvar->value = (flvalue);
	cvar->string = (char*)value.c_str();
	cvar->flags = FCVAR_SERVER;
	g_engfuncs.pfnCVarRegister(cvar);
	return g_engfuncs.pfnCVarGetPointer(name.c_str());
};
cvar_t* SvenEnhancerAs::GetCvar(CString& name)
{
	if (name.empty())
		return nullptr;
	return g_engfuncs.pfnCVarGetPointer(name.c_str());
};
bool SvenEnhancerAs::ClientCmdI(size_t index, CString& command)
{
	if (index <= 0)
		return false;
	return ClientCmd(INDEXENT(index), command);
}
SvenEnhancerEnt::SvenEnhancerEnt()
{
}
SvenEnhancerEnt::~SvenEnhancerEnt()
{
}

bool SvenEnhancerEnt::SetPDataInt(void* pThis, int offset, int value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}

bool SvenEnhancerEnt::SetPDataUInt(void* pThis, int offset, size_t value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}
bool SvenEnhancerEnt::SetPDataBool(void* pThis, int offset, bool value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}

bool SvenEnhancerEnt::SetPDataFloat(void* pThis, int offset, float value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}

bool SvenEnhancerEnt::SetPDataShort(void* pThis, int offset, short value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}

bool SvenEnhancerEnt::SetPDataUShort(void* pThis, int offset, unsigned short value, int linuxDiff)
{
	return PDataSetEnt(pThis, offset, value, linuxDiff);
}

int SvenEnhancerEnt::GetPDataInt(void* pThis, int offset, int linuxDiff)
{
	int result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

size_t SvenEnhancerEnt::GetPDataUInt(void* pThis, int offset, int linuxDiff)
{
	size_t result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

bool SvenEnhancerEnt::GetPDataBool(void* pThis, int offset, int linuxDiff)
{
	bool result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

float SvenEnhancerEnt::GetPDataFloat(void* pThis, int offset, int linuxDiff)
{
	float result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

short SvenEnhancerEnt::GetPDataShort(void* pThis, int offset, int linuxDiff)
{
	short result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

unsigned short SvenEnhancerEnt::GetPDataUShort(void* pThis, int offset, int linuxDiff)
{
	unsigned short result = 0;
	PDataGetEnt(pThis, offset, result, linuxDiff);
	return result;
}

void SvenEnhancerEnt::ClearPlayerData(int index)
{
	auto engine = GetASEngine();
	auto dictInfo = engine->GetTypeInfoByDecl("dictionary");
	auto it = g_PlayerData.find(index);
	if (it != g_PlayerData.end())
	{
		engine->ReleaseScriptObject(it->second, dictInfo);
		g_PlayerData.erase(it);
	}
}

void* SvenEnhancerEnt::GetData(int index)
{
	if (index < 0 || index > gpGlobals->maxEntities)
		return nullptr;
	std::unordered_map<int, void*>& target = index <= gpGlobals->maxClients ? g_PlayerData : g_EntityData;
	auto it = target.find(index);
	if (it != target.end())
		return it->second;
	auto dict = CreateDictionary(true);
	target[index] = dict;
	return dict;
}

void* SvenEnhancerEnt::GetDataByEdict(edict_t* edict)
{
	if (!edict || edict->free)
		return nullptr;
	int key = ENTINDEX(edict);
	return GetData(key);
}

void* SvenEnhancerEnt::GetDataByEntity(void* entity)
{
	if (!entity)
		return nullptr;
	entvars_t* pev = *((entvars_t**)((char*)entity + 4));
	if (!pev && !pev->pContainingEntity)
		return nullptr;
	auto key = ENTINDEX(ENT(pev));
	return GetData(key);
}

void SvenEnhancerEnt::ClearAllEntityData()
{
	auto engine = GetASEngine();
	auto dictInfo = engine->GetTypeInfoByDecl("dictionary");
	for (auto& [k, v] : g_EntityData)
	{
		engine->ReleaseScriptObject(v, dictInfo);
	}
	g_EntityData.clear();
}

bool SvenEnhancerEvent::On(CString& name, asIScriptFunction* callback)
{
	if (name.empty() || !callback)
		return false;
	callback->AddRef();
	std::string _name(name.c_str());
	auto parsed = ParseEvent(_name);
	auto& vec = g_events[parsed.name];
	vec.push_back(SvenEnhancerEventItem{
			callback,
			parsed.tag
		});
	return true;
}
bool SvenEnhancerEvent::Off(CString& name, asIScriptFunction* callback)
{
	if (name.empty())
		return false;
	std::string _name(name.c_str());
	ParsedEvent parsed = ParseEvent(_name);
	auto it = g_events.find(parsed.name);

	if (it == g_events.end())
		return false;

	auto& vec = it->second;
	for (auto i = vec.begin(); i != vec.end(); )
	{
		bool matchFn = (callback == nullptr || i->callback == callback);
		bool matchTag = (parsed.tag.empty() || i->tag == parsed.tag);
		if (matchFn && matchTag)
		{
			i = vec.erase(i);
		}
		else
		{
			++i;
		}
	}
	if (vec.empty())
		g_events.erase(it);

	return true;
}

size_t SvenEnhancerEvent::Trigger(CString& name, CallbackItem* item, bool callAll)
{
	if (name.empty())
		return 0;
	if (item)
	{
		//item->AddRef();

		CASServerManager* manager = ASEXT_GetServerManager();
		auto module = GetModulePtr(manager);
		if (module)
		{
			item->moduleName = module->GetName();
		}
	}
	std::string _name(name.c_str());
	auto it = g_events.find(_name);
	if (it == g_events.end())
		return 0;
	auto& vec = it->second;
	auto copy = vec;
	auto engine = GetASEngine();
	size_t total = 0;
	for (auto& entry : copy)
	{
		if (!entry.callback)
			continue;

		//item.StopCall = false;
		//item.ReturnCode = 0;
		asIScriptContext* ctx = engine->RequestContext();
		if (!ctx)
			continue;
		ctx->Prepare(entry.callback);
		ctx->SetArgAddress(0, &name);
		ctx->SetArgObject(1, item);
		int r = ctx->Execute();
		engine->ReturnContext(ctx);
		total++;
		if (item && item->StopCall && !callAll)
			break;
	}
	return total;
}

void SvenEnhancerEvent::ClearEventByModule(asIScriptModule* module)
{
	bool removedAny = false;
	for (auto it = g_events.begin(); it != g_events.end(); )
	{
		auto& vec = it->second;

		for (auto i = vec.begin(); i != vec.end(); )
		{
			if (i->callback && i->callback->GetModule() == module)
			{
				i->callback->Release();
				i->callback = nullptr;
				i = vec.erase(i);
				removedAny = true;
			}
			else
				++i;
		}
		if (vec.empty())
			it = g_events.erase(it);
		else
			++it;
	}
}
