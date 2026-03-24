
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

class CDictHelper {
public:
	CDictHelper(void* base);
	CScriptArray* getKeys();
	CScriptDictValue* getByName(CString& name);
	CString& getString(CString& name);
	void setByName(CString& name, void* obj, int typeId);
	bool keyExists(CString& name);
	std::string getStdString(CString& name, std::string fmt);
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
