#include <svenenhancer.h>
#include <mysql_sven.h>

SvenEnhancerAs::SvenEnhancerAs() {
	init();
}

void SvenEnhancerAs::init(){
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
	}
	CString* res = new CString();;
	res->assign(text.c_str(), text.length());
	//std::locale::global(old);
	return  res;
}
void SvenEnhancerAs::PluginExit()
{
	auto module = GetModulePtr(ASEXT_GetServerManager());
	if (module)
	{
		deleteModuleData(module);

	}
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



