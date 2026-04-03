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
	this->AddRef();
	return pDictionary->data;
}
JValue* SvenEnhancerAs::Json_ParseFromFile(CString& input) {
	this->AddRef();
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
	this->AddRef();
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
	this->AddRef();
	if (!obj || !type)
		return nullptr;
	JValue* jv = new JValue();
	jv->FillFromObject(obj, type);
	return jv;
}
JValue* SvenEnhancerAs::Json_ParseObjectV2(void* obj, int typeId)
{
	this->AddRef();
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
	this->AddRef();
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