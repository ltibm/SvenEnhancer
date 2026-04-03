#pragma once
#define JVALUE_TYPE_UNDEFINED  0
#define JVALUE_TYPE_OBJECT 1
#define JVALUE_TYPE_ARRAY 2
#define JVALUE_TYPE_STRING  3
#define JVALUE_TYPE_NUMBER  4
#define JVALUE_TYPE_BOOLEAN  5
#define JVALUE_TYPE_NULL  6
#define MAX_DEPTH 15
static asIScriptFunction* getFactoryForCreate(asITypeInfo*);
class JValue : public CASBaseGCObject
{
public:
	JValue();
	JValue(nlohmann::json& data);
	JValue(nlohmann::json& data, int _curDepth);
	unsigned int GetType();
	bool ContainsKey(CString& key);
	JValue* GetByKey(CString& key);
	JValue* Path(CString& key);
	CString* GetString();
	asINT32 GetInt();
	double GetDouble();
	bool GetBool();
	bool DeserializeB(void* obj, asITypeInfo* type);
	bool Deserialize(void* obj, int typeId);
	static void DeserializeC(asIScriptGeneric* gen);
	bool DeserializePlus(void** addr, int typeId);
	bool FillFromObject(void* obj, asITypeInfo* type);
	JValue* At(size_t index);
	unsigned int Size();
	bool Equals(const JValue& other);
	CString* ToString();
	CString* ToStringI(int indent);
	void SaveToFile(CString& path);
	void SaveToFileI(CString& path, int indent);
	//bool Set(void* obj, int typeId);
	//void SetNull();
	//For object
	bool SetByKey(CString& key, void* obj, int typeId);
	bool SetByPath(CString& key, void* obj, int typeId);
	bool SetByKeyNull(CString& key);
	bool SetByPathNull(CString& key);
	bool RemoveByKey(CString& key);
	bool RemoveByIndex(size_t index);
	bool RemoveByPath(CString& key);

	//For array
	bool SetByIndex(size_t index, void* obj, int typeId);
	bool SetByIndexNull(size_t index);

	//For array
	bool Add(void* obj, int typeId);
	//For Array
	bool AddNull();
	void Clear();
	void* GetKeys();
	size_t Count();
	~JValue();
	bool operator==(const JValue& other);
	//nlohmann::json& json;
	nlohmann::json json;
	JValue* parent = nullptr;
	asITypeInfo* GetMyTypeInfo();

private:
	asITypeInfo* myTypeInfo = nullptr;
	nlohmann::json FillFromObjectPrv(void* obj, asITypeInfo* type, int otherTypeId = 0, int depth = 0);
	int curDepth = 0;
};



extern int JOPT_READ;
extern int  JOPT_WRITE;
extern int JOPT_READWRITE;


class JSerializeConfig : public CASBaseGCObject
{
public:
	JSerializeConfig();
	JSerializeConfig* Ignore(CString& name);
	JSerializeConfig* IgnoreFlag(CString& name, int flags);
	JSerializeConfig* PropName(CString& name, CString& alias);
	JSerializeConfig* PropNameFlag(CString& name, CString& alias, int flags);
	typedef struct JPropInfo_s {
		std::string name;
		bool ignore;
	} JPropInfo;
	typedef struct JPropDef_s {
		JPropInfo* readProp;
		JPropInfo* writeProp;
	} JPropDef;
	JPropDef* GetPropDef(std::string name);
	JSerializeConfig* Clear();
	int Count();
	std::unordered_map<std::string, JPropDef*> propInfo;
private:
	JPropDef* getOrCreateDef(std::string name);
};


void RegisterJson(asIScriptEngine* engine);
void RegisterJValue(asIScriptEngine* engine);