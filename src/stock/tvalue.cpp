#include <svenenhancer.h>
#include "tvalue.h"

void RegisterTValue(asIScriptEngine* engine)
{
	int r = 0;
	engine->RegisterEnum("TvalueType");
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_UNDEFINED", TVALUE_TYPE_UNDEFINED);
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_OBJECT", TVALUE_TYPE_OBJECT);
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_ARRAY", TVALUE_TYPE_ARRAY);
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_STRING", TVALUE_TYPE_STRING);
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_NUMBER", TVALUE_TYPE_NUMBER);
	engine->RegisterEnumValue("TvalueType", "TVALUE_TYPE_BOOLEAN", TVALUE_TYPE_BOOLEAN);
	r = RegisterObject<TValue>("TValue", engine, asOBJ_REF | asOBJ_GC);
	r = engine->RegisterObjectMethod("TValue", "bool opEquals(const TValue &in other) const", asMETHOD(TValue, Equals), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "string& GetString(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "int GetInt(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "int64 GetInt64(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetInt64), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "float GetFloat(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetFloat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "double GetDouble(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetDouble), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "bool GetBool(string& in name = \"\", bool isPath = false) const", asMETHOD(TValue, GetBool), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetBool(string& in name, bool value) const", asMETHOD(TValue, SetBool), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetInt(string& in name, int value) const", asMETHOD(TValue, SetInt), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetInt64(string& in name, int64 value) const", asMETHOD(TValue, SetInt64), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetFloat(string& in name, float value) const", asMETHOD(TValue, SetFloat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetDouble(string& in name, double value) const", asMETHOD(TValue, SetDouble), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SetString(string& in name, string& in value) const", asMETHOD(TValue, SetString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "bool IsArray() const", asMETHOD(TValue, IsArray), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "TvalueType GetType() const", asMETHOD(TValue, GetType), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "string& ToString() const", asMETHOD(TValue, ToString), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void SaveToFile(const string& in filename) const", asMETHOD(TValue, SaveToFile), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "void Clear() const", asMETHOD(TValue, Clear), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "TValue@ At(uint index) const", asMETHOD(TValue, At), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "uint GetSize() const", asMETHOD(TValue, GetSize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("TValue", "TValue@ Get(string& in name, bool isPath = false) const", asMETHOD(TValue, Get), asCALL_THISCALL); assert(r >= 0);

}

CString* TValue::ToString()
{
	if (!node)
		return CreateString("");
	try
	{
		std::stringstream ss;
		ss << toml::default_formatter{ *node };
		std::string str = ss.str();
		return CreateString(str.c_str());
	}
	catch (...)
	{
	}

	return CreateString("");
}

void TValue::SaveToFile(CString& path, int indent)
{
	std::string filePath = path.c_str();
	if (filePath.ends_with(".as"))
		return;
	std::filesystem::path full = AsGetPath(filePath);
	try
	{
		std::ofstream file(full.c_str(), std::ios::out | std::ios::trunc);
		if (!file.is_open())
			return;
		file << toml::default_formatter{ *node };
	}
	catch (const std::exception&)
	{

	}
}
