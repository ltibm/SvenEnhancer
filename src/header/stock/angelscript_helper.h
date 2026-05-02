#pragma once
typedef CString dictKey_t;

class CScriptAny2 : public CASBaseGCObject
{
public:
	struct valueStruct
	{
		union
		{
			asINT64 valueInt;
			double  valueFlt;
			void* valueObj;
		};
		int   typeId;
	};
	int unk1;//0
	//int unk2;//4
	//int unk3;//8
	//int unk4;//12
	//int unk5;//16
	//int unk6;//20
	//int unk7;//20
	//int unk8;//20
	//asIScriptEngine* engine;
	valueStruct value;
	void Store(asIScriptEngine* engine, void* ref, int refTypeId);
	void FreeObject(asIScriptEngine* engine);
};
#pragma once
class CScriptDictValue
{
public:
	const void* GetAddressOfValue() const;
    int GetTypeId();
	friend class CScriptDictionary;

	union
	{
		asINT64 m_valueInt;
		double  m_valueFlt;
		void* m_valueObj;
	};
	int m_typeId;

};
#if AS_CAN_USE_CPP11
#include <unordered_map>
typedef std::unordered_map<dictKey_t, AS_NAMESPACE_QUALIFIER CScriptDictValue> dictMap_t;
#else
#include <map>
typedef std::map<dictKey_t, AS_NAMESPACE_QUALIFIER CScriptDictValue> dictMap_t;
#endif
#pragma pack(push, 1)
inline uint32_t GetHash(const char* str) {
	if (!str) return 0;
	uint32_t len = strlen(str);
	uint32_t hash = 0x811C9DC5;
	const uint32_t fnvPrime = 0x01000193;
	uint32_t step = (len / 10) + 1;
	for (uint32_t i = 0; i < len; i += step) {
		hash *= fnvPrime;
		hash ^= (uint8_t)str[i];
	}
	return hash;
}
struct DictionaryNode {
	DictionaryNode* pNext;
	DictionaryNode* pPrev;
	char szInlineKey[20];
	char* pszRefKey;
	uint8_t pad_to_value[16];
	union {
		int32_t   iVal;
		float     fVal;
		long long llVal;
		void* pPtr;
		double    dVal;
		uintptr_t uVal;
	} value;
	int32_t typeId;
};

struct CScriptDictinarySE {
	void* pVtable;
	void* pUnk;
	void* pUnk1;
	double dUnk;
	DictionaryNode* g_Iterator;
	int32_t size;
	DictionaryNode** ppBuckets;
	int32_t unk20; 
	int32_t unk24;
    int32_t bucketMask; 
    int32_t bucketCount;
};
#pragma pack(pop)
class CDictHelper {
public:
	CDictHelper(void* base);
	CScriptArray* getKeys();
	std::vector<std::string> getKeysV2();
	CScriptDictValue* getByName(std::string name);
	CScriptDictValue* getByName(CString& name);
	CString& getString(CString& name);
	void setByName(CString& name, void* obj, int typeId);
	bool keyExists(CString& name);
	std::string getStdString(CString& name, std::string fmt);
	size_t Size();
private:
	void* basePtr;
	asIScriptEngine* engine;
	asITypeInfo* dictType;
	asIScriptFunction* fnOpIndex;
	asIScriptFunction* fnGetKeys;
	asIScriptFunction* fnExists;
	asIScriptFunction* fnSet;
	asIScriptContext* initContext(asIScriptFunction* fn);
	int stringTypeId;
};



class CDateTimeHelper {
public:
	CDateTimeHelper(void* base);
	asUINT GetSeconds();
	asUINT GetMinutes();
	asUINT GetHour();
	asUINT GetDayOfMonth();
	asUINT GetMonth();
	asUINT GetYear();
	void SetSeconds(asUINT v);
	void SetMinutes(asUINT v);
	void SetHour(asUINT v);
	void SetDayOfMonth(asUINT v);
	void SetMonth(asUINT v);
	void SetYear(asUINT v);
	void SetDate(asUINT year, asUINT month, asUINT day);
	void SetTime(asUINT hour, asUINT min, asUINT sec);
	void FillFromUnix(std::time_t t);
	std::time_t ToUnix();
private:
	void* basePtr;
	asIScriptEngine* engine;
	asITypeInfo* dateTimeType;
	std::unordered_map<std::string, asIScriptFunction*> funcDefs;
	asIScriptContext* initContext(std::string fnName);
	asIScriptContext* initContext(asIScriptFunction* fn);
};

class CAnyHelper {

public:
	CAnyHelper(void* base);
	bool Retrieve(void* obj, int typeId);
	void Store(void* obj, int typeId);
private:
	void* basePtr;
	asIScriptEngine* engine;
	asITypeInfo* anyType;
	asIScriptFunction* fnRetrieve;
	asIScriptFunction* fnStore;
	asIScriptContext* initContext(asIScriptFunction* fn);
};


template <typename T>
int RegisterObject(const char* name, asIScriptEngine* engine, int flags)
{
	int r = 0;

	r = engine->RegisterObjectType(name, sizeof(T), flags); assert(r >= 0);

	if (flags & asOBJ_REF)
	{
		r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF,
			"void AddRef()", asMETHOD(T, AddRef), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE,
			"void Release()", asMETHOD(T, Release), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_SETGCFLAG,
			"void SetGCFlag()", asMETHOD(T, SetGCFlag), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETGCFLAG,
			"bool GetGCFlag() const", asMETHOD(T, GetGCFlag), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETREFCOUNT,
			"int GetRefCount() const", asMETHOD(T, GetRefCount), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_ENUMREFS,
			"void EnumReferences(int& in)", asMETHOD(T, EnumReferences), asCALL_THISCALL);

		r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASEREFS,
			"void ReleaseReferences(int& in)", asMETHOD(T, ReleaseReferences), asCALL_THISCALL);
	}

	return r;
}
static asITypeInfo* stringType = nullptr;
inline CScriptAny2* CreateAny()
{
	auto engine = GetASEngine();
	static asITypeInfo* anyType = engine->GetTypeInfoByDecl("any");
	auto any = engine->CreateScriptObject(anyType);
	CScriptAny2* s = reinterpret_cast<CScriptAny2*>(any);
	//CAnyHelper* helper = new CAnyHelper(s);
	engine->NotifyGarbageCollectorOfNewObject(any, anyType);
	return s;
}
template<typename T>
inline std::string GetASName();

template<>
inline std::string GetASName<int>() { return "int"; }

template<>
inline std::string GetASName<float>() { return "float"; }

template<>
inline std::string GetASName<std::string>() { return "string"; }
template<>
inline std::string GetASName<CString>() { return "string"; }

template<typename T>
struct ASArgSetter;

template<>
struct ASArgSetter<std::string>
{
	static void Set(asIScriptContext* ctx, const std::string& item)
	{
		CString* s = new CString();
		s->assign(item.c_str(), item.length());
		ctx->SetArgObject(0, s);
	}
};
template<>
struct ASArgSetter<CString>
{
	static void Set(asIScriptContext* ctx, const CString& item)
	{
		ctx->SetArgObject(0, (void*)&item);
	}
};
template<>
struct ASArgSetter<float>
{
	static void Set(asIScriptContext* ctx, float item)
	{
		ctx->SetArgFloat(0, item);
	}
};
template<>
struct ASArgSetter<int>
{
	static void Set(asIScriptContext* ctx, int item)
	{
		ctx->SetArgDWord(0, item);
	}
};

template<typename T>
void* CreateASArray_FromVector(const std::vector<T>& vec)
{
	std::string decl = "array<" + GetASName<T>() + ">";
	auto eng = GetASEngine();
	static asITypeInfo* arrInfo = eng->GetTypeInfoByDecl(decl.c_str());
	int typeId = arrInfo->GetTypeId();
	auto arr = eng->CreateScriptObject(arrInfo);
	eng->NotifyGarbageCollectorOfNewObject(arr, arrInfo);
	auto ctx = eng->RequestContext();
	auto method = arrInfo->GetMethodByName("insertLast");
	for (auto& item : vec) {
		ctx->Prepare(method);
		ctx->SetObject(arr);
		ASArgSetter<T>::Set(ctx, item);
		ctx->Execute();
		ctx->Unprepare();
	}
	eng->ReturnContext(ctx);
	return arr;
}
inline CString* CreateString(const char* text)
{
	CString* resp = new CString();
	if(!text)
	{
		resp->assign("", 0);
		return resp;
	}
	//auto engine = GetASEngine();
	//auto type = stringType ? stringType : stringType = engine->GetTypeInfoByName("string");
	//auto resp = static_cast<CString*>(engine->CreateScriptObject(type));
	resp->assign(text, strlen(text));
	return resp;
}

inline void* CreateDictionary(bool addRef = false)
{
	auto engine = GetASEngine();
	auto pDictionary = new RefObject();
	asITypeInfo* dictInfo = engine->GetTypeInfoByDecl("dictionary");
	pDictionary->type = dictInfo;
	pDictionary->data = engine->CreateScriptObject(dictInfo);
	if (addRef)
	{
		engine->AddRefScriptObject(pDictionary->data, dictInfo);
	}
	engine->NotifyGarbageCollectorOfNewObject(pDictionary->data, dictInfo);
	return pDictionary->data;
}