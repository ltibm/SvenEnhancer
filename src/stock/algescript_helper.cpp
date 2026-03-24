#include <svenenhancer.h>

#pragma region Dictionary
int CScriptDictValue::GetTypeId()
{
	return this->m_typeId;
}
const void* CScriptDictValue::GetAddressOfValue() const
{
	if ((m_typeId & asTYPEID_MASK_OBJECT) && !(m_typeId & asTYPEID_OBJHANDLE))
	{
		return m_valueObj;
	}
	return reinterpret_cast<const void*>(&m_valueObj);
}

CDictHelper::CDictHelper(void* base)
{
	this->basePtr = base;
	this->engine = GetASEngine();
	this->stringTypeId = engine->GetTypeIdByDecl("string");
	this->dictType = engine->GetTypeInfoByDecl("dictionary");
	this->fnOpIndex = this->dictType->GetMethodByDecl("dictionaryValue &opIndex(const string &in)");
	this->fnGetKeys = this->dictType->GetMethodByName("getKeys");
	this->fnExists = this->dictType->GetMethodByName("exists");
	this->fnSet = this->dictType->GetMethodByDecl("void set(const string&in, const ?&in)");
}
CScriptArray* CDictHelper::getKeys()
{
	if (!this->basePtr)
		return nullptr;
	auto ctx = initContext(this->fnGetKeys);
	if (ctx->Execute() == asEXECUTION_FINISHED)
	{
		return static_cast<CScriptArray*>(ctx->GetReturnObject());
	}
	return nullptr;
}
CScriptDictValue* CDictHelper::getByName(CString& name)
{
	if (!this->basePtr)
		return nullptr;
	auto ctx = this->initContext(this->fnOpIndex);
	ctx->SetArgObject(0, &name);
	if (ctx->Execute() == asEXECUTION_FINISHED)
	{
		auto obj = ctx->GetReturnAddress();
		return static_cast<CScriptDictValue*>(obj);
	}
	return nullptr;
}
CString& CDictHelper::getString(CString& name)
{
	// TODO: insert return statement here
	auto v = getByName(name);
	if (v && v->GetTypeId() == this->stringTypeId)
	{

		return *(CString*)v->GetAddressOfValue();
	}
}
void CDictHelper::setByName(CString& name, void* obj, int typeId)
{
	if (!this->basePtr || !this->fnSet)
		return;
	auto ctx = this->initContext(this->fnSet);
	ctx->SetArgObject(0, &name);
	ctx->SetArgVarType(1, obj, typeId);
	ctx->Execute();
}
bool CDictHelper::keyExists(CString& name)
{
	if (!this->basePtr)
		return false;
	auto ctx = this->initContext(this->fnExists);
	ctx->SetArgObject(0, &name);
	if (ctx->Execute() == asEXECUTION_FINISHED)
	{
		auto obj = ctx->GetReturnByte();
		return obj;
	}
	return false;
}
std::string CDictHelper::getStdString(CString& name, std::string fmt)
{
	auto v = this->getByName(name);
	if (v)
	{
		if (fmt.size() > 0)
		{
			fmt = "%" + fmt;
		}
		auto typeId = v->GetTypeId();
		char buffer[100];
		if (typeId == this->stringTypeId)
		{
			CString c = *(CString*)v->GetAddressOfValue();
			return c.c_str();
		}
		else if (typeId == asTYPEID_INT64)
		{
			asINT64 l;
			l = v->m_valueInt;
			//v->Get(this->engine, l);
			if (fmt.size() > 0)
			{
				sprintf(buffer, fmt.c_str(), l);
				return buffer;
			}
			return std::to_string(l);
		}
		else if (typeId == asTYPEID_DOUBLE)
		{
			double d;
			//v->Get(this->engine, d);
			d = v->m_valueFlt;
			if (fmt.size() > 0)
			{
				sprintf(buffer, fmt.c_str(), d);
				return buffer;
			}
			return std::to_string(d);
		}
	}
	return std::string();
}
asIScriptContext* CDictHelper::initContext(asIScriptFunction* fn)
{
	auto ctx = engine->RequestContext();
	ctx->Prepare(fn);
	ctx->SetObject(this->basePtr);
	return ctx;
}
#pragma endregion


#pragma region DateTime
CDateTimeHelper::CDateTimeHelper(void* base)
{
	basePtr = base;
	auto manager = ASEXT_GetServerManager();
	this->engine = manager->scriptEngine;
	this->dateTimeType = engine->GetTypeInfoByDecl("DateTime");
	this->funcDefs = {
		{"GetSeconds",  this->dateTimeType->GetMethodByName("GetSeconds")},
		{"GetMinutes",  this->dateTimeType->GetMethodByName("GetMinutes")},
		{"GetHour",  this->dateTimeType->GetMethodByName("GetHour")},
		{"GetDayOfMonth",  this->dateTimeType->GetMethodByName("GetDayOfMonth")},
		{"GetMonth",  this->dateTimeType->GetMethodByName("GetMonth")},
		{"GetYear",  this->dateTimeType->GetMethodByName("GetYear")},
		{"SetSeconds",  this->dateTimeType->GetMethodByName("SetSeconds")},
		{"SetMinutes",  this->dateTimeType->GetMethodByName("SetMinutes")},
		{"SetHour",  this->dateTimeType->GetMethodByName("SetHour")},
		{"SetDayOfMonth",  this->dateTimeType->GetMethodByName("SetDayOfMonth")},
		{"SetMonth",  this->dateTimeType->GetMethodByName("SetMonth")},
		{"SetYear",  this->dateTimeType->GetMethodByName("SetYear")}
	};
}
asUINT CDateTimeHelper::GetSeconds()
{
	auto ctx = this->initContext("GetSeconds");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}
asUINT CDateTimeHelper::GetMinutes()
{
	auto ctx = this->initContext("GetMinutes");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}
asUINT CDateTimeHelper::GetHour()
{
	auto ctx = this->initContext("GetHour");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}
asUINT CDateTimeHelper::GetDayOfMonth()
{
	auto ctx = this->initContext("GetDayOfMonth");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}
asUINT CDateTimeHelper::GetMonth()
{
	auto ctx = this->initContext("GetMonth");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}
asUINT CDateTimeHelper::GetYear()
{
	auto ctx = this->initContext("GetYear");
	ctx->SetObject(this->basePtr);
	if (ctx->Execute() == asEXECUTION_FINISHED)
		return ctx->GetReturnDWord();
	return asUINT();
}


void CDateTimeHelper::SetSeconds(asUINT v)
{
	auto ctx = this->initContext("SetSeconds");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetMinutes(asUINT v)
{
	auto ctx = this->initContext("SetMinutes");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetHour(asUINT v)
{
	auto ctx = this->initContext("SetHour");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetDayOfMonth(asUINT v)
{
	auto ctx = this->initContext("SetDayOfMonth");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetMonth(asUINT v)
{
	auto ctx = this->initContext("SetMonth");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetYear(asUINT v)
{
	auto ctx = this->initContext("SetYear");
	ctx->SetObject(this->basePtr);
	ctx->SetArgDWord(0, v);
	ctx->Execute();
}
void CDateTimeHelper::SetDate(asUINT year, asUINT month, asUINT day)
{
	SetYear(year);
	SetMonth(month);
	SetDayOfMonth(day);
}
void CDateTimeHelper::SetTime(asUINT hour, asUINT min, asUINT sec)
{
	SetHour(hour);
	SetMinutes(min);
	SetSeconds(sec);
}

void CDateTimeHelper::FillFromUnix(std::time_t t)
{
	std::tm* timeInfo = std::gmtime(&t);
	this->SetDate(timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
	this->SetTime(timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
}

std::time_t CDateTimeHelper::ToUnix()
{
	std::tm timeInfo = {};
	timeInfo.tm_year = this->GetYear() - 1900;
	timeInfo.tm_mon = this->GetMonth() - 1;
	timeInfo.tm_mday = this->GetDayOfMonth();
	timeInfo.tm_hour = this->GetHour();
	timeInfo.tm_min = this->GetMinutes();
	timeInfo.tm_sec = this->GetSeconds();
	return static_cast<std::time_t>(std::mktime(&timeInfo));
}

asIScriptContext* CDateTimeHelper::initContext(std::string fnName)
{
	return initContext(funcDefs[fnName]);
}

asIScriptContext* CDateTimeHelper::initContext(asIScriptFunction* fn)
{
	auto ctx = engine->RequestContext();
	ctx->Prepare(fn);
	ctx->SetObject(this->basePtr);
	return ctx;
}
#pragma endregion


#pragma region Any
CAnyHelper::CAnyHelper(void* base)
{
	this->basePtr = base;
	CASServerManager* manager = ASEXT_GetServerManager();
	this->engine = manager->scriptEngine;
	this->anyType = engine->GetTypeInfoByDecl("any");
	this->fnStore = this->anyType->GetMethodByDecl("void store(const ?&in)");
	this->fnRetrieve = this->anyType->GetMethodByDecl("bool retrieve(const ?&out)");
}

bool CAnyHelper::Retrieve(void* obj, int typeId)
{
	auto ctx = initContext(this->fnRetrieve);
	ctx->SetArgVarType(0, obj, typeId);
	if (ctx->Execute() == asEXECUTION_FINISHED)
	{
		return ctx->GetReturnByte() != 0;
	}
	return false;
}

void CAnyHelper::Store(void* obj, int typeId)
{
	auto ctx = initContext(this->fnStore);
	ctx->SetArgVarType(0, obj, typeId);
	if (ctx->Execute() == asEXECUTION_FINISHED)
	{
		
	}
}


asIScriptContext* CAnyHelper::initContext(asIScriptFunction* fn)
{
	auto ctx = engine->RequestContext();
	ctx->Prepare(fn);
	ctx->SetObject(this->basePtr);
	return ctx;
}

#pragma endregion


void CScriptAny2::FreeObject(asIScriptEngine* engine)
{
	// If it is a handle or a ref counted object, call release
	if (value.typeId & asTYPEID_MASK_OBJECT)
	{
		// Let the engine release the object
		asITypeInfo* ti = engine->GetTypeInfoById(value.typeId);
		engine->ReleaseScriptObject(value.valueObj, ti);

		// Release the object type info
		if (ti)
			ti->Release();

		value.valueObj = 0;
		value.typeId = 0;
	}

	// For primitives, there's nothing to do
}

void CScriptAny2::Store(asIScriptEngine* engine, void* ref, int refTypeId)
{
	// This method is not expected to be used for primitive types, except for bool, int64, or double
	assert(refTypeId > asTYPEID_DOUBLE || refTypeId == asTYPEID_VOID || refTypeId == asTYPEID_BOOL || refTypeId == asTYPEID_INT64 || refTypeId == asTYPEID_DOUBLE);

	// Hold on to the object type reference so it isn't destroyed too early
	if ((refTypeId & asTYPEID_MASK_OBJECT))
	{
		asITypeInfo* ti = engine->GetTypeInfoById(refTypeId);
		if (ti)
			ti->AddRef();
	}
	FreeObject(engine);
	value.typeId = refTypeId;
	if (value.typeId & asTYPEID_OBJHANDLE)
	{
		// We're receiving a reference to the handle, so we need to dereference it
		value.valueObj = *(void**)ref;
		engine->AddRefScriptObject(value.valueObj, engine->GetTypeInfoById(value.typeId));
	}
	else if (value.typeId & asTYPEID_MASK_OBJECT)
	{
		// Create a copy of the object
		value.valueObj = engine->CreateScriptObjectCopy(ref, engine->GetTypeInfoById(value.typeId));
	}
	else
	{
		// Primitives can be copied directly
		value.valueInt = 0;

		// Copy the primitive value
		// We receive a pointer to the value.
		int size = engine->GetSizeOfPrimitiveType(value.typeId);
		memcpy(&value.valueInt, ref, size);
	}
}


