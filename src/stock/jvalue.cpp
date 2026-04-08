#include <svenenhancer.h>
#include "jvalue.h"

int JOPT_READ = 1;
int JOPT_WRITE = 2;
int JOPT_READWRITE = JOPT_READ | JOPT_WRITE;

JValue::JValue() : json(*new nlohmann::json())
{

}
JValue::JValue(nlohmann::json& _json) : json(_json)
{

}
JValue::JValue(nlohmann::json& _json, int _curDepth) : json(_json)
{
	curDepth = _curDepth;

}
unsigned int JValue::GetType()
{
	if (json.is_object())
		return JVALUE_TYPE_OBJECT;
	else if (json.is_array())
		return JVALUE_TYPE_ARRAY;
	else if (json.is_string())
		return JVALUE_TYPE_STRING;
	else if (json.is_number())
		return JVALUE_TYPE_NUMBER;
	else if (json.is_boolean())
		return JVALUE_TYPE_BOOLEAN;
	else if (json.is_null())
		return JVALUE_TYPE_NULL;
	return JVALUE_TYPE_UNDEFINED;
}

bool JValue::ContainsKey(CString& key)
{
	std::string str = key.c_str();
	return json.contains(str);
}

JValue* JValue::GetByKey(CString& key)
{
	std::string str = key.c_str();
	if (!json.contains(str))
	{
		return nullptr;
	}
	nlohmann::json jv = json.at(str);
	JValue* j = new JValue(jv);
	return j;
}

JValue* JValue::Path(CString& key)
{
	std::string str = std::string(key.c_str());
	try
	{
		nlohmann::json& jv = json.at(nlohmann::json::json_pointer(str));
		JValue* j = new JValue(jv);
		return j;
	}
	catch (const std::exception&)
	{
		return nullptr;
	}
}

CString* JValue::GetString()
{
	std::string str = TryGetJson<std::string>();
	CString* cstr = new CString();
	cstr->assign(str.c_str(), str.length());
	return cstr;
}

CString* JValue::GetStringB(CString& key, bool isPath)
{	
	auto str = GetValueOrDefault<std::string>(key.c_str(), isPath);
	return CreateString(str.c_str());
}

asINT32 JValue::GetInt()
{
	return TryGetJson<asINT32>();
}

asINT32 JValue::GetIntB(CString& key, bool isPath)
{
	return GetValueOrDefault<asINT32>(key.c_str(), isPath);
}

float JValue::GetFloat()
{
	return TryGetJson<float>();
}

float JValue::GetFloatB(CString& key, bool isPath)
{
	return GetValueOrDefault<float>(key.c_str(), isPath);
}

double JValue::GetDouble()
{
	return TryGetJson<double>();
}
double JValue::GetDoubleB(CString& key, bool isPath)
{
	return GetValueOrDefault<double>(key.c_str(), isPath);
}

bool JValue::GetBool()
{
	return TryGetJson<bool>();
}
bool JValue::GetBoolB(CString& key, bool isPath)
{
	return GetValueOrDefault<bool>(key.c_str(), isPath);
}
template <typename T>
T TransformValue(const nlohmann::json& jvalue) {
	if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
		if (jvalue.is_number_integer()) {
			return static_cast<T>(jvalue.get<int64_t>());
		}
		else if (jvalue.is_string()) {
			return static_cast<T>(std::stoll(jvalue.get<std::string>()));
		}
	}
	else if constexpr (std::is_floating_point_v<T>) {
		if (jvalue.is_number()) {
			return static_cast<T>(jvalue.get<double>());
		}
		else if (jvalue.is_string()) {
			return static_cast<T>(std::stod(jvalue.get<std::string>()));
		}
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		if (jvalue.is_string()) {
			return jvalue.get<std::string>();
		}
		else if (jvalue.is_number()) {
			return std::to_string(jvalue.get<double>());
		}
		else if (jvalue.is_boolean()) {
			return jvalue.get<bool>() ? "true" : "false";
		}
	}
	else if constexpr (std::is_same_v<T, bool>) {
		if (jvalue.is_boolean()) {
			return jvalue.get<bool>();
		}
		else if (jvalue.is_number()) {
			return jvalue.get<double>() != 0;
		}
		else if (jvalue.is_string()) {
			std::string s = jvalue.get<std::string>();
			return (s == "true" || s == "1");
		}
	}
	throw std::invalid_argument("Cannot convert JSON value to requested type");
}

template <typename T>
void setOffset(nlohmann::json t, void* obj, int offset)
{
	auto tVal = TransformValue<T>(t);
	*((T*)((uintptr_t)obj + offset)) = (T)tVal;
}
template <typename T>
T getOffsetValue(void* obj, int offset)
{
	T value;
	std::memcpy(&value, static_cast<char*>(obj) + offset, sizeof(T));
	return value;
}

template <typename T>
T getValueNoOffset(void* obj)
{
	T value;
	std::memcpy(&value, static_cast<char*>(obj), sizeof(T));
	return value;
}


void setJValueFromValue(int typeId, nlohmann::json& json, std::string name, void* obj)
{
	if (typeId == asTYPEID_BOOL)
		json[name] = getValueNoOffset<bool>(obj);
	else if (typeId == asTYPEID_INT8)
		json[name] = getValueNoOffset<asINT8>(obj);
	else if (typeId == asTYPEID_INT16)
		json[name] = getValueNoOffset<asINT16>(obj);
	else if (typeId == asTYPEID_INT32)
		json[name] = getValueNoOffset<asINT32>(obj);
	else if (typeId == asTYPEID_INT64)
		json[name] = getValueNoOffset<asINT64>(obj);
	else if (typeId == asTYPEID_UINT8)
		json[name] = getValueNoOffset<unsigned char>(obj);
	else if (typeId == asTYPEID_UINT16)
		json[name] = getValueNoOffset<unsigned short>(obj);
	else if (typeId == asTYPEID_UINT32)
		json[name] = getValueNoOffset<unsigned int>(obj);
	else if (typeId == asTYPEID_UINT64)
		json[name] = getValueNoOffset<unsigned long long>(obj);
	else if (typeId == asTYPEID_FLOAT)
		json[name] = getValueNoOffset<float>(obj);
	else if (typeId == asTYPEID_DOUBLE)
		json[name] = getValueNoOffset<double>(obj);
}

void setJValueFromOffset(int typeId, nlohmann::json& json, std::string name, void* obj, int offset)
{
	if (typeId == asTYPEID_BOOL)
		json[name] = getOffsetValue<bool>(obj, offset);
	else if (typeId == asTYPEID_INT8)
		json[name] = getOffsetValue<asINT8>(obj, offset);
	else if (typeId == asTYPEID_INT16)
		json[name] = getOffsetValue<asINT16>(obj, offset);
	else if (typeId == asTYPEID_INT32)
		json[name] = getOffsetValue<asINT32>(obj, offset);
	else if (typeId == asTYPEID_INT64)
		json[name] = getOffsetValue<asINT64>(obj, offset);
	else if (typeId == asTYPEID_UINT8)
		json[name] = getOffsetValue<unsigned char>(obj, offset);
	else if (typeId == asTYPEID_UINT16)
		json[name] = getOffsetValue<unsigned short>(obj, offset);
	else if (typeId == asTYPEID_UINT32)
		json[name] = getOffsetValue<unsigned int>(obj, offset);
	else if (typeId == asTYPEID_UINT64)
		json[name] = getOffsetValue<unsigned long long>(obj, offset);
	else if (typeId == asTYPEID_FLOAT)
		json[name] = getOffsetValue<float>(obj, offset);
	else if (typeId == asTYPEID_DOUBLE)
		json[name] = getOffsetValue<double>(obj, offset);
}


//Only primvite types
void setOffset(int typeId, nlohmann::json& t, void* obj, int offset)
{
	if (typeId == asTYPEID_BOOL)
		setOffset<bool>(t, obj, offset);
	else if (typeId == asTYPEID_INT8)
		setOffset<asINT8>(t, obj, offset);
	else if (typeId == asTYPEID_INT16)
		setOffset<asINT16>(t, obj, offset);
	else if (typeId == asTYPEID_INT32)
		setOffset<asINT32>(t, obj, offset);
	else if (typeId == asTYPEID_INT64)
		setOffset<asINT64>(t, obj, offset);
	else if (typeId == asTYPEID_UINT8)
		setOffset<unsigned char>(t, obj, offset);
	else if (typeId == asTYPEID_UINT16)
		setOffset<unsigned short>(t, obj, offset);
	else if (typeId == asTYPEID_UINT32)
		setOffset<unsigned int>(t, obj, offset);
	else if (typeId == asTYPEID_UINT64)
		setOffset<unsigned long long>(t, obj, offset);
	else if (typeId == asTYPEID_FLOAT)
		setOffset<float>(t, obj, offset);
	else if (typeId == asTYPEID_DOUBLE)
		setOffset<double>(t, obj, offset);
}


void* getJValue(int typeId, nlohmann::json& t, asIScriptEngine* engine, int curDepth)
{
	if (typeId == asTYPEID_BOOL)
	{
		return new bool(TransformValue<bool>(t));
	}
	else if (typeId == asTYPEID_INT8)
	{
		return new asINT8(TransformValue<asINT8>(t));
	}
	else if (typeId == asTYPEID_INT16)
	{
		return new asINT16(TransformValue<asINT16>(t));
	}
	else if (typeId == asTYPEID_INT32)
	{
		return new asINT32(TransformValue<asINT32>(t));
	}
	else if (typeId == asTYPEID_INT64)
	{
		return new asINT64(TransformValue<asINT64>(t));
	}
	else if (typeId == asTYPEID_UINT8)
	{
		return new unsigned char(TransformValue<unsigned char>(t));
	}
	else if (typeId == asTYPEID_UINT16)
	{
		return new unsigned short(TransformValue<unsigned short>(t));
	}
	else if (typeId == asTYPEID_UINT32)
	{
		return new unsigned int(TransformValue<unsigned int>(t));

	}
	else if (typeId == asTYPEID_UINT64)
	{
		return new unsigned long long(TransformValue<unsigned long long>(t));
	}
	else if (typeId == asTYPEID_FLOAT)
	{
		return new float(TransformValue<float>(t));
	}
	else if (typeId == asTYPEID_DOUBLE)
	{
		return new double(TransformValue<double>(t));
	}
	else if (typeId == AS_TYPEID_STRING)
	{
		CString* s = new CString();
		if (t.is_string())
		{

			s->assign(t.get<std::string>().c_str(), t.get<std::string>().length());
		}
		else if (t.is_number_unsigned())
		{
			auto str = std::to_string(t.get<unsigned long long>());
			s->assign(str.c_str(), str.length());
		}
		else if (t.is_number_float())
		{
			auto str = std::to_string(t.get<double>());
			s->assign(str.c_str(), str.length());
		}
		else if (t.is_number())
		{
			auto str = std::to_string(t.get<double>());
			s->assign(str.c_str(), str.length());
		}
		else if (t.is_boolean())
		{
			auto str = std::to_string(t.get<bool>());
			s->assign(str.c_str(), str.length());
		}
		return s;
	}
	else if (typeId == AS_TYPEID_STRING_T)
	{
		auto strIndex = ALLOC_STRING(TransformValue<std::string>(t).c_str());
		return new string_t(strIndex);

	}
	else if (typeId == AS_TYPEID_DICTIONARY)
	{
		if (t.is_object())
		{
			asITypeInfo* dictInfo = engine->GetTypeInfoByDecl("dictionary");
			auto dict = engine->CreateScriptObject(dictInfo);
			engine->NotifyGarbageCollectorOfNewObject(dict, dictInfo);
			auto jv = new JValue(t, curDepth + 1);
			jv->DeserializeB(dict, dictInfo);
			return dict;
		}
	}
	else if (typeId == AS_TYPEID_DATETIME)
	{
		if (t.is_number())
		{
			asITypeInfo* dateType = engine->GetTypeInfoByDecl("DateTime");
			auto dt = engine->CreateScriptObject(dateType);
			engine->NotifyGarbageCollectorOfNewObject(dt, dateType);
			auto* dateHelper = new CDateTimeHelper(dt);
			dateHelper->FillFromUnix(t);
			return dt;
		}
	}
	else if (typeId == AS_TYPEID_ANY_HANDLE || typeId == AS_TYPEID_ANY)
	{
		int typeId = 0;
		if (t.is_number_unsigned())
		{
			typeId = asTYPEID_UINT64;
		}
		else if (t.is_number_float())
		{
			typeId = asTYPEID_DOUBLE;
		}
		else if (t.is_number())
		{
			typeId = asTYPEID_INT64;
		}
		else if (t.is_boolean())
		{
			typeId = asTYPEID_BOOL;
		}
		else if (t.is_string())
		{
			typeId = AS_TYPEID_STRING;
		}
		else if (t.is_object())
		{
			typeId = AS_TYPEID_DICTIONARY;
		}
		else if (t.is_array())
		{
			typeId = AS_TYPEID_ARRAY_ANY_HANDLE;
		}
		if (typeId > 0)
		{




			asITypeInfo* anyType = engine->GetTypeInfoByDecl("any");
			//asITypeInfo* anyType = engine->GetTypeInfoByDecl("any");
			auto any = engine->CreateScriptObject(anyType);
			CScriptAny2* s = reinterpret_cast<CScriptAny2*>(any);
			CAnyHelper* helper = new CAnyHelper(s);

			//CScriptAny* s_0 = reinterpret_cast<CScriptAny*>(any);
			engine->NotifyGarbageCollectorOfNewObject(any, anyType);
			auto* obj = getJValue(typeId, t, engine, curDepth);
			s->Store(engine, obj, typeId);
			return s;
			/*
			CScriptAny2* s = reinterpret_cast<CScriptAny2*>(any);

			CScriptAny2::valueStruct* vs = new CScriptAny2::valueStruct();
			vs->typeId = typeId;
			if (isNumericType(typeId))
			{
				if (typeId == asTYPEID_FLOAT || typeId == asTYPEID_DOUBLE)
				{
					vs->valueFlt = *reinterpret_cast<double*>(obj);
				}
				else {
					vs->valueInt = *reinterpret_cast<asINT64*>(obj);
				}
			}
			else {
				vs->valueObj = obj;
			}
			s->value = *vs;
			return s;*/

		}
	}
	else if (typeId != asTYPEID_VOID) {
		if (t.is_object() || t.is_array())
		{
			JValue* sub = new JValue(t, curDepth + 1);
			auto info = engine->GetTypeInfoById(typeId);
			auto factory = getFactoryForCreate(info);
			if (factory)
			{
				auto ctx = engine->RequestContext();
				ctx->Prepare(factory);
				ctx->Execute();
				void* obj = ctx->GetReturnObject();
				void* addr = ctx->GetReturnAddress();
				if (obj)
				{
					sub->DeserializeB(obj, info);
					//ctx->Release();
					return addr;
				}
				else {
					//ctx->Release();
				}
			}
		}
	}
	return nullptr;
}

static asIScriptFunction* getFactoryForCreate(asITypeInfo* type)
{
	if (!type)
		return nullptr;
	size_t fct = type->GetFactoryCount();
	for (size_t i = 0; i < fct; i++)
	{
		auto fn = type->GetFactoryByIndex(i);
		if (fn->GetParamCount() == 0)
			return fn;
	}
	return nullptr;
}

bool JValue::DeserializeB(void* obj, asITypeInfo* type) {
	if (curDepth >= MAX_DEPTH)
		return false;

	if (!type)
		return false;
	auto eng = type->GetEngine();
	int typeId = type->GetTypeId();
	auto c = type;
	if (!json.is_array() && !json.is_object())
		return false;
	if (isPrimviteType(typeId))
		return false;

	asIScriptFunction* fnCOnfig = type->GetMethodByDecl("void __JSConfig(JSerializeConfig@ config)");
	auto typeName = type->GetName();
	if (strcmp(typeName, "array") == 0)
	{
		if (json.is_array())
		{
			auto method = type->GetMethodByName("insertLast");
			CScriptArray* s = reinterpret_cast<CScriptArray*>(obj);
			if (s && method)
			{
				int elementTypeId = s->m_ElementTypeId;
				for (auto it = json.begin(); it != json.end(); ++it)
				{
					nlohmann::json jcur = it.value();

					auto ptr = getJValue(elementTypeId, jcur, eng, curDepth);
					if (ptr)
					{
						auto ctx = eng->RequestContext();
						ctx->Prepare(method);
						ctx->SetObject(s);
						if (elementTypeId == asTYPEID_INT32)
						{
							//void* obj = *reinterpret_cast<void**>(static_cast<uint8_t*>(ptr));
							ctx->SetArgAddress(0, ptr);
						}
						else {

							if (elementTypeId & asTYPEID_OBJHANDLE)
							{
								ctx->SetArgAddress(0, &ptr);
							}
							else {
								ctx->SetArgAddress(0, ptr);
							}
						}
						ctx->Execute();
						//ctx->Release();
					}
				}
			}
			return true;
		}
		return false;
	}
	if (typeId == AS_TYPEID_DICTIONARY)
	{
		if (json.is_object())
		{
			auto dictHelper = new CDictHelper(obj);
			for (auto it = json.begin(); it != json.end(); ++it)
			{
				CString* key = new CString();
				key->assign(it.key().c_str(), it.key().length());
				nlohmann::json subValue = it.value();
				if (subValue.is_number_unsigned())
				{
					unsigned long long numB = subValue;
					dictHelper->setByName(*key, &numB, asTYPEID_UINT64);
				}
				else if (subValue.is_number_integer())
				{
					int numB = subValue;
					dictHelper->setByName(*key, &numB, asTYPEID_INT32);
				}
				else if (subValue.is_number_float())
				{
					float numB = subValue;
					dictHelper->setByName(*key, &numB, asTYPEID_FLOAT);
				}
				else if (subValue.is_number())
				{
					double numB = subValue;
					dictHelper->setByName(*key, &numB, asTYPEID_DOUBLE);
				}
				else if (subValue.is_string())
				{
					std::string var = subValue;
					CString* v = new CString();
					v->assign(var.c_str(), var.length());
					dictHelper->setByName(*key, v, AS_TYPEID_STRING);
				}
				else if (subValue.is_object())
				{
					asITypeInfo* dictInfo = eng->GetTypeInfoByDecl("dictionary");
					auto dict = eng->CreateScriptObject(dictInfo);
					eng->NotifyGarbageCollectorOfNewObject(dict, dictInfo);
					auto jvSub = new JValue(subValue, curDepth + 1);
					jvSub->DeserializeB(dict, dictInfo);
					dictHelper->setByName(*key, dict, dictInfo->GetTypeId());
				}
				else if (subValue.is_array())
				{

					asITypeInfo* arrInfo = eng->GetTypeInfoByDecl("array<any@>");
					int typeId = arrInfo->GetTypeId();
					auto arr = eng->CreateScriptObject(arrInfo);
					eng->NotifyGarbageCollectorOfNewObject(arr, arrInfo);
					auto jvSub = new JValue(subValue, curDepth + 1);
					jvSub->DeserializeB(arr, arrInfo);
					dictHelper->setByName(*key, arr, typeId);

				}
			}
			return true;
		}
		return false;
	}
	JSerializeConfig* config = new JSerializeConfig();
	if (fnCOnfig)
	{
		auto ctxProp = eng->RequestContext();
		ctxProp->Prepare(fnCOnfig);
		ctxProp->SetObject(obj);
		ctxProp->SetArgObject(0, config);
		ctxProp->Execute();
		ctxProp->Release();
	}
	int methodCount = c->GetMethodCount();
	for (size_t i = 0; i < methodCount; i++)
	{
		auto function = c->GetMethodByIndex(i);
		if (!function->IsProperty() || function->IsPrivate() || function->IsProtected())
			continue;
		std::string name = function->GetName();
		std::string nameProp;
		if (name.rfind("set_", 0) != 0) {
			continue;
		}
		nameProp = name.substr(4);
		int typeId;
		asDWORD flag;
		function->GetParam(0, &typeId, &flag);
		auto propInfo = config->GetPropDef(nameProp);
		std::string _alias;
		if (propInfo)
		{
			if (propInfo->readProp->ignore)
				continue;
			_alias = propInfo->readProp->name;
		}
		if (!json.contains(_alias.size() > 0 ? _alias : nameProp))
		{
			continue;
		}
		if (!typeId)
			continue;



		//nlohmann::json t = json.at(_alias.size() > 0 ? _alias : nameProp);
		nlohmann::json t = json[_alias.size() > 0 ? _alias : nameProp];
		//void* jvl = getJValue(typeId, t, eng, curDepth);
		auto ctx = eng->RequestContext();
		int prp = ctx->Prepare(function);
		ctx->SetObject(obj);
		if (typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE)
		{
			if (typeId == asTYPEID_BOOL || typeId == asTYPEID_INT8 || typeId == asTYPEID_UINT8)
			{
				ctx->SetArgByte(0, TransformValue<asBYTE>(t));
			}
			else if (typeId == asTYPEID_INT16 || typeId == asTYPEID_UINT16)
			{
				ctx->SetArgWord(0, TransformValue<asWORD>(t));
			}
			else if (typeId == asTYPEID_INT32 || typeId == asTYPEID_UINT32)
			{
				ctx->SetArgDWord(0, TransformValue<asDWORD>(t));
			}
			else if (typeId == asTYPEID_INT64 || typeId == asTYPEID_UINT64)
			{
				ctx->SetArgQWord(0, TransformValue<asQWORD>(t));
			}
			else if (typeId == asTYPEID_FLOAT)
			{
				ctx->SetArgFloat(0, TransformValue<float>(t));
			}
			else if (typeId == asTYPEID_DOUBLE)
			{
				ctx->SetArgDouble(0, TransformValue<double>(t));
			}
		}
		else {
			void* jvl = getJValue(typeId, t, eng, curDepth);
			if (typeId & asTYPEID_OBJHANDLE)
			{
				ctx->SetArgAddress(0, &jvl);
			}
			else {
				ctx->SetArgObject(0, jvl);

			}
		}
		int exRes = ctx->Execute();
		//ctx->Release();
	}
	int propertyCount = c->GetPropertyCount();
	for (size_t i = 0; i < propertyCount; i++)
	{
		const char* name; int typeId; bool isPrivate;
		int offset; bool isProtected; bool isReference;
		c->GetProperty(i, &name, &typeId, &isPrivate, &isProtected, &offset, &isReference);

		auto propInfo = config->GetPropDef(name);
		std::string _alias;
		if (propInfo)
		{
			if (propInfo->readProp->ignore)
				continue;
			_alias = propInfo->readProp->name;
		}

		if (isPrivate || isProtected)
			continue;
		if (!json.contains(_alias.size() > 0 ? _alias : name))
		{
			continue;
		}
		//nlohmann::json t = json.at(_alias.size() > 0 ? _alias : name);
		nlohmann::json t = json[_alias.size() > 0 ? _alias : name];
		if (typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE)
		{
			setOffset(typeId, t, obj, offset);
		}
		else if (typeId == AS_TYPEID_STRING)
		{
			CString* s = *reinterpret_cast<CString**>(static_cast<uint8_t*>(obj) + offset);

			if (!s)
				continue;

			std::string str = TransformValue<std::string>(t);
			s->assign(str.c_str(), str.length());
		}
		else if (typeId == AS_TYPEID_STRING_T)
		{
			auto strIndex = ALLOC_STRING(TransformValue<std::string>(t).c_str());
			*((string_t*)((uintptr_t)obj + offset)) = (string_t)strIndex;
		}
		else if (typeId == AS_TYPEID_DICTIONARY)
		{
			if (t.is_object())
			{
				void* dict = *reinterpret_cast<CScriptArray**>(static_cast<uint8_t*>(obj) + offset);
				asITypeInfo* dictInfo = eng->GetTypeInfoByDecl("dictionary");
				if (!dict)
				{
					dict = eng->CreateScriptObject(dictInfo);
					eng->NotifyGarbageCollectorOfNewObject(dict, dictInfo);
					*reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset) = &dict;
				}
				auto jvSub = new JValue(t, curDepth + 1);
				jvSub->DeserializeB(dict, dictInfo);
			}
		}
		else if (typeId == AS_TYPEID_DATETIME)
		{
			if (t.is_number())
			{
				void* datetime = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset);
				auto* dateHelper = new CDateTimeHelper(datetime);
				dateHelper->FillFromUnix(t);
			}
		}
		else if (typeId != asTYPEID_VOID)
		{
			auto info = eng->GetTypeInfoById(typeId);
			if (t.is_array() || t.is_object())
			{
				JValue* sub = new JValue(t, curDepth + 1);
				void* newObj = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset);
				if (newObj)
				{
					sub->Deserialize(newObj, typeId);
				}
				else {
					JValue* sub = new JValue(t, curDepth + 1);
					auto info = eng->GetTypeInfoById(typeId);
					auto n = info->GetName();
					auto factory = getFactoryForCreate(info);
					if (factory)
					{
						auto ctx = eng->RequestContext();
						ctx->Prepare(factory);
						ctx->Execute();
						void* newObjSub = ctx->GetReturnAddress();
						//ctx->Release();
						if (newObjSub)
						{
							sub->DeserializeB(newObjSub, info);
							*reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset) = newObjSub;
						}
					}
				}
			}
		}
	}
	return true;
}

//DEPRECATED
void JValue::DeserializeC(asIScriptGeneric* gen)
{
	if (!gen)
		return;
	JValue* self = reinterpret_cast<JValue*>(gen->GetObject());
	if (!self) return;
	void* addr = gen->GetAddressOfArg(0);
	void** addr2 = (void**)gen->GetAddressOfArg(0);
	bool r = false;
	if (addr)
	{
		int typeId = gen->GetArgTypeId(0);
		auto eng = ASEXT_GetServerManager()->scriptEngine;
		auto c = eng->GetTypeInfoById(typeId);
		void* ref = addr;

		if (typeId & asTYPEID_OBJHANDLE)
		{
			ref = *(void**)addr;
		}
		r = self->DeserializeB(ref, c);

	}

	*(bool*)gen->GetAddressOfReturnLocation() = r;
}
bool JValue::DeserializePlus(void** addr, int typeId)
{
	void* obj = *addr;
	if (typeId & asTYPEID_OBJHANDLE)
	{
		obj = *(void**)obj;
	}
	auto eng = ASEXT_GetServerManager()->scriptEngine;
	auto c = eng->GetTypeInfoById(typeId);
	if (!c)
		return false;
	bool result = DeserializeB(obj, c);
	//*addr = obj;
	return result;
}

bool JValue::Deserialize(void* obj, int typeId)
{
	if (!obj || !typeId)
		return false;
	if (typeId & asTYPEID_OBJHANDLE)
	{
		obj = *(void**)obj;
	}
	auto eng = ASEXT_GetServerManager()->scriptEngine;

	auto c = eng->GetTypeInfoById(typeId);
	if (!c)
		return false;
	bool result = DeserializeB(obj, c);

	return result;
}

bool JValue::FillFromObject(void* obj, asITypeInfo* type)
{
	if (!type)
		return false;
	json = FillFromObjectPrv(obj, type);
	return true;
}

nlohmann::json JValue::FillFromObjectPrv(void* obj, asITypeInfo* type, int otherTypeId, int depth)
{
	int baseTypeId = otherTypeId > 0 ? otherTypeId : type->GetTypeId();
	if (isPrimviteType(baseTypeId))
	{
		if (baseTypeId == asTYPEID_BOOL)
			return *reinterpret_cast<bool*>(obj);
		else if (baseTypeId == asTYPEID_INT8)
			return *reinterpret_cast<asINT8*>(obj);
		else if (baseTypeId == asTYPEID_INT16)
			return *reinterpret_cast<asINT16*>(obj);
		else if (baseTypeId == asTYPEID_INT32)
			return *reinterpret_cast<asINT32*>(obj);
		else if (baseTypeId == asTYPEID_INT64)
			return *reinterpret_cast<asINT64*>(obj);
		else if (baseTypeId == asTYPEID_UINT8)
			return *reinterpret_cast<unsigned char*>(obj);
		else if (baseTypeId == asTYPEID_UINT16)
			return *reinterpret_cast<unsigned short*>(obj);
		else if (baseTypeId == asTYPEID_UINT32)
			return *reinterpret_cast<unsigned int*>(obj);
		else if (baseTypeId == asTYPEID_UINT64)
			return *reinterpret_cast<unsigned long long*>(obj);
		else if (baseTypeId == asTYPEID_FLOAT)
			return *reinterpret_cast<float*>(obj);
		else if (baseTypeId == asTYPEID_DOUBLE)
			return *reinterpret_cast<double*>(obj);
		else if (baseTypeId = AS_TYPEID_STRING)
		{
			CString* s = reinterpret_cast<CString*>(obj);
			std::string str = s->c_str();
			return str;
		}
		else if (baseTypeId == AS_TYPEID_STRING_T)
		{
			string_t index = *(string_t*)(static_cast<uint8_t*>(obj));
			char* str = (char*)g_engfuncs.pfnSzFromIndex(index);
			return str;
		}
		else if (baseTypeId == AS_TYPEID_DATETIME)
		{
			void* datetime = *reinterpret_cast<void**>(obj);
			auto* dateHelper = new CDateTimeHelper(datetime);
			return dateHelper->ToUnix();
		}
		return nullptr;
	}

	//Maximum depth
	if (depth >= MAX_DEPTH)
		return nullptr;

	if (type == GetMyTypeInfo())
	{
		auto otherJv = static_cast<JValue*>(obj);
		nlohmann::json& jv = otherJv->json;
		return jv;
	}

	auto c = type;
	auto eng = type->GetEngine();


	asIScriptFunction* fnCOnfig = type->GetMethodByDecl("void __JSConfig(JSerializeConfig@ config)");
	auto typeName = type->GetName();
	if (strcmp(typeName, "array") == 0)
	{
		auto method = type->GetMethodByName("insertLast");
		auto methodCount = type->GetMethodByName("size");

		//opIndex
		auto methodOp = type->GetMethodByIndex(0);
		CScriptArray* s = reinterpret_cast<CScriptArray*>(obj);

		if (s)
		{
			nlohmann::json jarray = nlohmann::json::array();

			int elementTypeId = s->m_ElementTypeId;
			auto arrT = eng->GetTypeInfoById(elementTypeId);

			auto ctxsz = eng->RequestContext();
			ctxsz->Prepare(methodCount);
			ctxsz->SetObject(s);
			ctxsz->Execute();

			size_t size = ctxsz->GetReturnDWord();
			//ctxsz->Release();
			for (size_t i = 0; i < size; i++)
			{
				auto ctx = eng->RequestContext();
				ctx->Prepare(methodOp);
				ctx->SetObject(s);
				ctx->SetArgDWord(0, i);
				if (ctx->Execute() != asEXECUTION_FINISHED)
					continue;
				void* obj = ctx->GetReturnAddress();
				void* objb = ctx->GetReturnObject();

				void* newObj = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj));
				jarray.push_back(this->FillFromObjectPrv(isPrimviteType(elementTypeId) || isSpecialType(elementTypeId) ? obj : newObj, arrT, arrT == nullptr ? elementTypeId : 0, depth++));
			}
			return jarray;
		}
		return nullptr;
	}
	if (baseTypeId == AS_TYPEID_DICTIONARY)
	{
		CDictHelper* dictHelper = new CDictHelper(obj);
		nlohmann::json jobject = nlohmann::json::object();
		auto keys = dictHelper->getKeys();
		auto size = keys->size();
		for (size_t i = 0; i < size; i++)
		{

			auto key = static_cast<CString*>(Array_At(keys, i));
			CString* cstr = new CString();
			cstr->assign(key->c_str(), strlen(key->c_str()));
			auto dictValue = dictHelper->getByName(*cstr);
			if (dictValue)
			{
				void* addr = (void*)dictValue->GetAddressOfValue();
				auto dTypeId = dictValue->GetTypeId();
				auto dType = eng->GetTypeInfoById(dTypeId);
				void* newObj = *reinterpret_cast<void**>(static_cast<uint8_t*>(addr));
				jobject[key->c_str()] = this->FillFromObjectPrv(isPrimviteType(dTypeId) || isSpecialType(dTypeId) ? addr : addr, dType, dType == nullptr ? dTypeId : 0, depth++);
				//jobject[key->c_str()] = this->FillFromObjectPrv(newObj, dType, dType == nullptr ? dictValue->GetTypeId() : 0);
			}
		}
		return jobject;
	}
	if (baseTypeId == AS_TYPEID_ANY)
	{
		CScriptAny2* s = reinterpret_cast<CScriptAny2*>(obj);
		if (s)
		{
			int anyId = s->value.typeId;
			if (isNumericType(anyId))
			{
				if (anyId == asTYPEID_DOUBLE || anyId == asTYPEID_FLOAT)
				{
					return s->value.valueFlt;
				}
				else {
					return s->value.valueInt;
				}
			}
			void* ref = s->value.valueObj;
			auto dType = eng->GetTypeInfoById(anyId);
			return FillFromObjectPrv(ref, dType, dType == nullptr ? anyId : 0, depth++);
		}


	}
	JSerializeConfig* config = new JSerializeConfig();
	if (fnCOnfig)
	{
		auto ctxProp = eng->RequestContext();
		ctxProp->Prepare(fnCOnfig);
		ctxProp->SetObject(obj);
		ctxProp->SetArgObject(0, config);
		ctxProp->Execute();
		ctxProp->Release();
	}
	nlohmann::json json = nlohmann::json::object();
	int methodCount = c->GetMethodCount();
	for (size_t i = 0; i < methodCount; i++)
	{
		auto function = c->GetMethodByIndex(i);
		if (!function->IsProperty() || function->IsPrivate() || function->IsProtected())
			continue;
		std::string name = function->GetName();
		std::string nameProp;
		if (name.rfind("get_", 0) != 0) {
			continue;
		}
		nameProp = name.substr(4);
		int typeId = function->GetReturnTypeId();
		auto propInfo = config->GetPropDef(nameProp);
		std::string _alias;
		if (propInfo)
		{
			if (propInfo->writeProp->ignore)
				continue;
			_alias = propInfo->writeProp->name;
		}
		else {
			_alias = nameProp;
		}
		if (_alias.size() == 0)
			_alias = name;
		auto mtype = eng->GetTypeInfoById(typeId);
		auto ctx = eng->RequestContext();
		ctx->Prepare(function);
		ctx->SetObject(obj);
		int exRes = ctx->Execute();
		if (typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE)
		{
			if (typeId == asTYPEID_BOOL)
			{
				json[_alias] = ctx->GetReturnByte() != 0;
			}
			else if (typeId == asTYPEID_INT8 || typeId == asTYPEID_UINT8)
			{
				json[_alias] = ctx->GetReturnByte();
			}
			else if (typeId == asTYPEID_INT16 || typeId == asTYPEID_UINT16)
			{
				json[_alias] = ctx->GetReturnWord();
			}
			else if (typeId == asTYPEID_INT32 || typeId == asTYPEID_UINT32)
			{
				json[_alias] = ctx->GetReturnDWord();
			}
			else if (typeId == asTYPEID_INT64 || typeId == asTYPEID_UINT64)
			{
				json[_alias] = ctx->GetReturnQWord();
			}
			else if (typeId == asTYPEID_FLOAT)
			{
				json[_alias] = ctx->GetReturnFloat();
			}
			else if (typeId == asTYPEID_DOUBLE)
			{
				json[_alias] = ctx->GetReturnDouble();
			}

		}
		else if (typeId != asTYPEID_VOID)
		{
			void* rObj = ctx->GetReturnAddress();

			if (typeId & asTYPEID_OBJHANDLE)
			{
				void* actualObj = *(void**)rObj;
				if (actualObj)
					json[_alias] = this->FillFromObjectPrv(actualObj, mtype, 0, depth++);
				else
					json[_alias] = nullptr;
			}
			else
			{
				if (rObj)
					json[_alias] = this->FillFromObjectPrv(rObj, mtype, 0, depth++);
				else
					json[_alias] = nullptr;
			}

		}
		//ctx->Release();
	}
	int propertyCount = c->GetPropertyCount();
	auto btypeName = type->GetName();

	for (size_t i = 0; i < propertyCount; i++)
	{
		const char* name; int typeId; bool isPrivate;
		int offset; bool isProtected; bool isReference;
		c->GetProperty(i, &name, &typeId, &isPrivate, &isProtected, &offset, &isReference);
		auto propInfo = config->GetPropDef(name);
		std::string _alias;
		if (propInfo)
		{
			if (propInfo->writeProp->ignore)
				continue;
			_alias = propInfo->writeProp->name;
		}
		else {
			_alias = name;
		}
		if (_alias.size() == 0)
			_alias = name;
		if (isPrivate || isProtected)
			continue;

		if (typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE)
		{
			setJValueFromOffset(typeId, json, _alias, obj, offset);
		}
		else if (typeId == AS_TYPEID_STRING)
		{
			CString* s = *reinterpret_cast<CString**>(static_cast<uint8_t*>(obj) + offset);
			if (!s)
				continue;
			std::string _str = s->c_str();
			json[_alias] = _str;


		}
		else if (typeId == AS_TYPEID_STRING_T)
		{
			string_t index = *(string_t*)(static_cast<uint8_t*>(obj) + offset);
			char* str = (char*)g_engfuncs.pfnSzFromIndex(index);
			json[_alias] = str;
		}
		else if (typeId == AS_TYPEID_DICTIONARY)
		{
			void* newObj = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset);
			asITypeInfo* dictInfo = eng->GetTypeInfoByDecl("dictionary");
			json[_alias] = this->FillFromObjectPrv(newObj, dictInfo, 0, depth++);
		}
		else if (typeId == AS_TYPEID_DATETIME)
		{
			void* datetime = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset);
			auto* dateHelper = new CDateTimeHelper(datetime);
			json[_alias] = dateHelper->ToUnix();
		}
		else if (typeId != asTYPEID_VOID)
		{
			auto info = eng->GetTypeInfoById(typeId);
			auto subTypeName = info->GetName();
			bool isarray = strcmp(typeName, "array") == 0;
			//JValue* sub = new JValue(t, curDepth + 1);
			void* newObj = *reinterpret_cast<void**>(static_cast<uint8_t*>(obj) + offset);
			if (newObj)
			{
				if (newObj)
				{
					json[_alias] = this->FillFromObjectPrv(newObj, info, 0, depth++);
				}
				else {
					json[_alias] = nullptr;
				}
			}
		}
	}
	return json;
}



JValue* JValue::At(size_t index)
{
	try
	{
		nlohmann::json& jv = json.at(index);
		JValue* j = new JValue(jv);
		return j;
	}
	catch (const std::exception&)
	{
		return nullptr;
	}
}

unsigned int JValue::Size()
{
	return json.size();
}

bool JValue::Equals(const JValue& other)
{
	return json == other.json;
}

CString* JValue::ToString()
{
	return ToStringI(-1);
}

CString* JValue::ToStringI(int indent)
{
	CString* str = new CString();
	auto dumped = json.dump(indent);
	str->assign(dumped.c_str(), dumped.size());
	return str;
}

CString* JValue::ToStringB(CString& key, bool isPath, int indent)
{
	if (key.size() == 0)
		return ToStringI(indent);
	try
	{
		nlohmann::json& jv = isPath ? json.at(nlohmann::json::json_pointer(key.c_str())) : json.at(key.c_str());
		CString* str = new CString();
		auto dumped = jv.dump(indent);
		str->assign(dumped.c_str(), dumped.size());
		return str;
	}
	catch (const std::exception&)
	{
			
	}
	return CreateString("");
}

void JValue::SaveToFile(CString& path)
{
	SaveToFileI(path, -1);
}
void JValue::SaveToFileI(CString& path, int indent)
{
	std::string filePath = path.c_str();
	if (filePath.ends_with(".as"))
		return;
	std::filesystem::path _path(filePath);
	std::filesystem::path rootPath = "./svencoop";
	std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
	try
	{
		std::ofstream out(full);
		out << json.dump(indent);
	}
	catch (const std::exception&)
	{

	}
}
//bool JValue::Set(void* obj, int typeId)
//{
//	if (typeId & asTYPEID_OBJHANDLE)
//		obj = *(void**)obj;
//	if (!obj)
//		return false;
//	auto type = GetASEngine()->GetTypeInfoById(typeId);
//	this->Clear();
//	json = FillFromObjectPrv(obj, type, typeId, 0);
//	return true;
//}
//void JValue::SetNull()
//{
//
//}
bool JValue::SetByKey(CString& key, void* obj, int typeId)
{
	if (!json.is_object())
		return false;
	if (typeId & asTYPEID_OBJHANDLE)
		obj = *(void**)obj;
	if (!obj)
		return false;


	auto type = GetASEngine()->GetTypeInfoById(typeId);

	auto jv =   FillFromObjectPrv(obj, type, typeId, 0);
	try
	{
		json[key.c_str()] = jv;	
		return true;

	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::SetByPath(CString& key, void* obj, int typeId)
{
	if (!json.is_object() && !json.is_array())
		return false;
	if (typeId & asTYPEID_OBJHANDLE)
		obj = *(void**)obj;
	if (!obj)
		return false;
	auto type = GetASEngine()->GetTypeInfoById(typeId);
	auto jv = FillFromObjectPrv(obj, type, typeId, 0);
	try
	{
		json[nlohmann::json::json_pointer(key.c_str())] = jv;
		return true;

	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::SetByKeyNull(CString& key)
{
	if (!json.is_object())
		return false;
	try
	{
		json[key.c_str()] = nullptr;
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::SetByPathNull(CString& key)
{
	if (!json.is_object() && !json.is_array())
		return false;
	try
	{
		json[nlohmann::json::json_pointer(key.c_str())] = nullptr;
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::RemoveByIndex(size_t index)
{
	if (!json.is_array() || index < 0)
		return false;
	try
	{
		json.erase(index);
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::RemoveByPath(CString& key)
{
	if (!json.is_object() && !json.is_array())
		return false;
	try
	{
		json.erase(nlohmann::json::json_pointer(key.c_str()));
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::RemoveByKey(CString& key)
{
	if (!json.is_object())
		return false;
	try
	{
		json.erase(key.c_str());
		return true;
	}
	catch (const std::exception&)
	{
			
	}
	return false;
}
bool JValue::SetByIndex(size_t index, void* obj, int typeId)
{
	if (!json.is_array() || index < 0)
		return false;
	if (typeId & asTYPEID_OBJHANDLE)
		obj = *(void**)obj;
	if (!obj)
		return false;
	auto type = GetASEngine()->GetTypeInfoById(typeId);
	auto jv = FillFromObjectPrv(obj, type, typeId, 0);
	try
	{
		json[index] = jv;
		return true;

	}
	catch (const std::exception&)
	{
	}
	return false;
}
bool JValue::SetByIndexNull(size_t index)
{
	if (!json.is_array() || index < 0)
		return false;
	try
	{
		json[index] = nullptr;
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool JValue::Add(void* obj, int typeId)
{
	if (!json.is_array())
		return false;
	if (typeId & asTYPEID_OBJHANDLE)
		obj = *(void**)obj;
	if (!obj)
		return false;
	auto type = GetASEngine()->GetTypeInfoById(typeId);
	auto jv = FillFromObjectPrv(obj, type, typeId, 0);
	try
	{
		json.push_back(jv);
		return true;

	}
	catch (const std::exception&)
	{
	}
	return false;
}
bool JValue::AddNull()
{
	if (!json.is_array())
		return false;
	try
	{
		json.push_back(nullptr);
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
void JValue::Clear()
{
	json.clear();
}

void* JValue::GetKeys()
{
	auto eng = GetASEngine();
	asITypeInfo* arrInfo = eng->GetTypeInfoByDecl("array<string>");
	int typeId = arrInfo->GetTypeId();
	auto arr = eng->CreateScriptObject(arrInfo);
	eng->NotifyGarbageCollectorOfNewObject(arr, arrInfo);
	if(!json.is_object())
		return arr;
	auto method = arrInfo->GetMethodByName("insertLast");
	for (auto& [key, value] : json.items()) {
		CString* s = new CString();
		s->assign(key.c_str(), key.length());
		auto ctx = eng->RequestContext();
		ctx->Prepare(method);
		ctx->SetObject(arr);
		ctx->SetArgObject(0, s);
		ctx->Execute();
	}
	auto it = json.begin();
	return arr;
}

size_t JValue::Count()
{
	if (json.is_array() || json.is_object())
		return json.size();
	return 0;
}

JValue::~JValue()
{
	json = nlohmann::json(nullptr);
}


bool JValue::operator==(const JValue& other)
{
	return Equals(other);
}

asITypeInfo* JValue::GetMyTypeInfo()
{
	if (!myTypeInfo)
	{
		auto eng = GetASEngine();
		if(eng)
			myTypeInfo = eng->GetTypeInfoByName("JValue");
	}
	return myTypeInfo;
}

void RegisterJson(asIScriptEngine* engine) {
	RegisterJValue(engine);
}


static void JValue_Construct(void* mem)
{
	new(mem) JValue();
}

static void JValue_Destruct(JValue* obj)
{
	obj->Release();
	obj->~JValue();
}



void RegisterJValue(asIScriptEngine* engine)
{

	int thisCall = 3;
	int objLast = 4;


	int r = engine->RegisterGlobalProperty("const int JOPT_READ", &JOPT_READ);
	r = engine->RegisterGlobalProperty("const int JOPT_WRITE", &JOPT_WRITE);
	r = engine->RegisterGlobalProperty("const int JOPT_READWRITE", &JOPT_READWRITE);
	engine->RegisterEnum("JvalueType");
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_UNDEFINED", JVALUE_TYPE_UNDEFINED);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_OBJECT", JVALUE_TYPE_OBJECT);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_ARRAY", JVALUE_TYPE_ARRAY);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_STRING", JVALUE_TYPE_STRING);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_NUMBER", JVALUE_TYPE_NUMBER);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_BOOLEAN", JVALUE_TYPE_BOOLEAN);
	engine->RegisterEnumValue("JvalueType", "JVALUE_TYPE_NULL", JVALUE_TYPE_NULL);


	r = RegisterObject<JValue>("JValue", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("JValue", "bool opEquals(const JValue &in other) const", asMETHOD(JValue, Equals), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "string& GetString() const", asMETHOD(JValue, GetString), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "string& GetString(string& in name, bool isPath = false) const", asMETHOD(JValue, GetStringB), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "JValue@ GetByKey(string& in input) const", asMETHOD(JValue, GetByKey), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool ContainsKey(string& in input) const", asMETHOD(JValue, ContainsKey), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "JValue@ Path(string &in input) const", asMETHOD(JValue, Path), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "uint Size() const", asMETHOD(JValue, Size), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "JValue@ At(uint index) const", asMETHOD(JValue, At), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool GetBool() const", asMETHOD(JValue, GetBool), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool GetBool(string& in name, bool isPath = false) const", asMETHOD(JValue, GetBoolB), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "int GetInt() const", asMETHOD(JValue, GetInt), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "int GetInt(string& in name, bool isPath = false) const", asMETHOD(JValue, GetIntB), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "float GetFloat() const", asMETHOD(JValue, GetFloat), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "float GetFloat(string& in name, bool isPath = false) const", asMETHOD(JValue, GetFloatB), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "double GetDouble() const", asMETHOD(JValue, GetDouble), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "double GetDouble(string& in name, bool isPath = false) const", asMETHOD(JValue, GetDoubleB), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "string& ToString() const", asMETHOD(JValue, ToString), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "string& ToString(int indent) const", asMETHOD(JValue, ToStringI), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "string& ToString(string& in name, bool isPath = false, int indent = -1) const", asMETHOD(JValue, ToStringB), thisCall); assert(r >= 0);

	r = engine->RegisterObjectMethod("JValue", "void SaveToFile(string& in path) ", asMETHOD(JValue, SaveToFile), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "void SaveToFile(string& in path, int indent) ", asMETHOD(JValue, SaveToFileI), thisCall); assert(r >= 0);
	//r = engine->RegisterObjectMethod("JValue", "bool Deserialize(?&out)", asFUNCTION(JValue::DeserializeC), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool Deserialize(?&in)", asMETHOD(JValue, Deserialize), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool Set(string&in key, ?&in)", asMETHOD(JValue, SetByKey), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool SetByIndex(uint index, ?&in)", asMETHOD(JValue, SetByIndex), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool SetByPath(string&in key, ?&in)", asMETHOD(JValue, SetByPath), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool SetNull(string&in key)", asMETHOD(JValue, SetByKeyNull), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool SetNullByIndex(uint index)", asMETHOD(JValue, SetByIndexNull), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool SetNullByPath(string&in key)", asMETHOD(JValue, SetByPathNull), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool Add(?&in)", asMETHOD(JValue, Add), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool AddNull(?&in)", asMETHOD(JValue, AddNull), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool Remove(string&in key)", asMETHOD(JValue, RemoveByKey), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool RemoveByIndex(uint index)", asMETHOD(JValue, RemoveByIndex), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "bool RemoveByPath(string&in key)", asMETHOD(JValue, RemoveByPath), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "uint Count()", asMETHOD(JValue, Count), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "array<string>& GetKeys()", asMETHOD(JValue, GetKeys), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JValue", "JvalueType GetType()", asMETHOD(JValue, GetType), thisCall); assert(r >= 0);


	//r = engine->RegisterObjectMethod("JValue", "bool Deserialize(?&out)", asMETHOD(JValue, DeserializePlus), thisCall); assert(r >= 0);


	r = engine->RegisterObjectMethod("JValue", "void Clear() const", asMETHOD(JValue, Clear), thisCall); assert(r >= 0);

	/*
		JValue End
	*/

	/*
	JSerializeConfig Start
	*/

	r = RegisterObject<JSerializeConfig>("JSerializeConfig", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("JSerializeConfig", "JSerializeConfig@ Ignore(string& in name, int flags = JOPT_READWRITE) const", asMETHOD(JSerializeConfig, IgnoreFlag), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JSerializeConfig", "JSerializeConfig@ PropName(string& in name, string& in _alias, int flags = JOPT_READWRITE) const", asMETHOD(JSerializeConfig, PropNameFlag), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JSerializeConfig", "JSerializeConfig@ Clear() const", asMETHOD(JSerializeConfig, Clear), thisCall); assert(r >= 0);
	r = engine->RegisterObjectMethod("JSerializeConfig", "int Count() const", asMETHOD(JSerializeConfig, Count), thisCall); assert(r >= 0);


	/*
	JSerializeConfig End
	*/
}

JSerializeConfig::JSerializeConfig()
{
}

JSerializeConfig* JSerializeConfig::Ignore(CString& name)
{
	return IgnoreFlag(name, JOPT_READ | JOPT_WRITE);
}

JSerializeConfig* JSerializeConfig::IgnoreFlag(CString& name, int flags)
{
	this->AddRef();
	JPropDef* def = getOrCreateDef(name.c_str());
	if ((flags & JOPT_READ) == JOPT_READ)
	{
		def->readProp->ignore = true;
	}
	if ((flags & JOPT_WRITE) == JOPT_WRITE)
	{
		def->writeProp->ignore = true;
	}
	return this;
}

JSerializeConfig* JSerializeConfig::PropName(CString& name, CString& alias)
{
	return PropNameFlag(name, alias, JOPT_READ | JOPT_WRITE);
}

JSerializeConfig* JSerializeConfig::PropNameFlag(CString& _name, CString& _alias, int flags)
{
	this->AddRef();
	std::string alias = _alias.c_str();
	JPropDef* def = getOrCreateDef(_name.c_str());
	if ((flags & JOPT_READ) == JOPT_READ)
	{
		def->readProp->name = alias.c_str();
	}
	if ((flags & JOPT_WRITE) == JOPT_WRITE)
	{
		def->writeProp->name = alias.c_str();
	}
	return this;
}

JSerializeConfig::JPropDef* JSerializeConfig::GetPropDef(std::string name)
{
	auto it = this->propInfo.find(name);
	if (it != this->propInfo.end())
		return it->second;
	return nullptr;
}

JSerializeConfig* JSerializeConfig::Clear()
{
	this->AddRef();
	this->propInfo.clear();
	return this;
}

int JSerializeConfig::Count()
{
	return this->propInfo.size();
}

JSerializeConfig::JPropDef* JSerializeConfig::getOrCreateDef(std::string name)
{
	JPropDef* def = nullptr;
	auto it = this->propInfo.find(name);
	if (it == propInfo.end())
	{
		this->propInfo[name] = def = new JPropDef();
	}
	else {
		def = it->second;
	}
	if (!def->readProp)
		def->readProp = new JPropInfo();
	if (!def->writeProp)
		def->writeProp = new JPropInfo();
	return def;
}
