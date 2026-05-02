#pragma once
#include <filesystem>
inline asIScriptEngine* GetASEngine() {
	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptEngine* engine = manager->scriptEngine;
	return engine;
}
enum class ArgType
{
	Int,
	UInt64,
	Bool,
	Float,
	Double,
	Object,
	String
};

struct DynamicArg
{
	ArgType type;
	int intVal{};
	uint64_t qwordVal{};
	bool boolVal{};
	float floatVal{};
	double doubleVal{};
	void* objVal{};
	std::string strVal{};
};

inline bool AS_SetDynamicArg(asIScriptContext* ctx,
	asUINT index,
	int typeId,
	const DynamicArg& arg)
{
	if (typeId == asTYPEID_INT32 && arg.type == ArgType::Int)
		return ctx->SetArgDWord(index, arg.intVal) >= 0;

	if (typeId == asTYPEID_UINT64 && arg.type == ArgType::UInt64)
		return ctx->SetArgQWord(index, arg.qwordVal) >= 0;

	if (typeId == asTYPEID_BOOL && arg.type == ArgType::Bool)
		return ctx->SetArgByte(index, arg.boolVal ? 1 : 0) >= 0;

	if (typeId == asTYPEID_FLOAT && arg.type == ArgType::Float)
		return ctx->SetArgFloat(index, arg.floatVal) >= 0;

	if (typeId == asTYPEID_DOUBLE && arg.type == ArgType::Double)
		return ctx->SetArgDouble(index, arg.doubleVal) >= 0;

	if ((typeId & asTYPEID_MASK_OBJECT) && arg.type == ArgType::Object)
		return ctx->SetArgObject(index, arg.objVal) >= 0;
	if ((typeId & AS_TYPEID_STRING) && arg.type == ArgType::String)
	{
		CString* str = new CString();
		str->assign(arg.strVal.c_str(), arg.strVal.size());
		return ctx->SetArgObject(index,str) >= 0;

	}
	return false;
}
inline bool AS_IsFunctionSignatureValid(asIScriptFunction* func,
	const std::string& sig,
	const std::string& returnType = "")
{
	if (!func)
		return false;

	struct ExpectedParam
	{
		std::string type;
		bool isRef = false;
		bool isHandle = false;
	};

	std::vector<ExpectedParam> expected;

	std::stringstream ss(sig);
	std::string item;

	while (std::getline(ss, item, ','))
	{
		// trim
		while (!item.empty() && std::isspace((unsigned char)item.front()))
			item.erase(item.begin());
		while (!item.empty() && std::isspace((unsigned char)item.back()))
			item.pop_back();

		if (item.empty())
			continue;
		size_t lastSpace = item.find_last_of(' ');
		if (lastSpace != std::string::npos)
		{
			std::string last = item.substr(lastSpace + 1);

			if (last.find('&') == std::string::npos &&
				last.find("::") == std::string::npos)
			{
				item = item.substr(0, lastSpace);

				while (!item.empty() &&
					std::isspace((unsigned char)item.back()))
				{
					item.pop_back();
				}
			}
		}

		ExpectedParam p;
		if (item.rfind("ref ", 0) == 0)
		{
			p.isRef = true;
			item = item.substr(4);
		}

		if (item.rfind("const ", 0) == 0)
		{
			item = item.substr(6);
		}

		size_t handlePos = item.find('@');
		if (handlePos != std::string::npos)
		{
			p.isHandle = true;
			//item.erase(handlePos, 1);
		}

		size_t refPos = item.find('&');
		if (refPos != std::string::npos)
		{
			p.isRef = true;
			item = item.substr(0, refPos);

			while (!item.empty() &&
				std::isspace((unsigned char)item.back()))
				item.pop_back();
		}

		// final trim
		while (!item.empty() && std::isspace((unsigned char)item.front()))
			item.erase(item.begin());
		while (!item.empty() && std::isspace((unsigned char)item.back()))
			item.pop_back();

		p.type = item;

		expected.push_back(p);
	}

	if (func->GetParamCount() != expected.size())
		return false;

	for (asUINT i = 0; i < func->GetParamCount(); ++i)
	{
		int typeId;
		asDWORD flags;
		const char* name = nullptr;
		const char* def = nullptr;

		if (func->GetParam(i, &typeId, &flags, &name, &def) < 0)
			return false;

		const auto& exp = expected[i];

		std::string realType =
			func->GetEngine()->GetTypeDeclaration(typeId);

		bool realIsHandle = (typeId & asTYPEID_OBJHANDLE) != 0;

		bool realIsRef =
			(flags & asTM_INREF) ||
			(flags & asTM_OUTREF) ||
			(flags & asTM_INOUTREF);

		if (realType != exp.type)
			return false;

		if (realIsHandle != exp.isHandle)
			return false;

		if (realIsRef != exp.isRef)
			return false;
	}
	if (!returnType.empty())
	{
		int retTypeId = func->GetReturnTypeId();

		std::string realReturn =
			func->GetEngine()->GetTypeDeclaration(retTypeId);

		if (realReturn != returnType)
			return false;
	}

	return true;
}
inline asITypeInfo* GetASGlobalTypeByName(const char* name)
{
	asIScriptEngine* engine = GetASEngine();
	auto tc = engine->GetObjectTypeCount();
	for (size_t i = 0; i < tc; i++)
	{
		auto type = engine->GetObjectTypeByIndex(i);
		if (strcmp(type->GetName(), name) == 0)
			return type;
	}
	return nullptr;
}

inline int AS_GetTypeId(asIScriptEngine* engine, std::string name, bool isdecl = false)
{
	auto r = isdecl ? engine->GetTypeInfoByDecl(name.c_str()) : engine->GetTypeInfoByName(name.c_str());
	if (r)
	{
		int tid = r->GetTypeId();
		return tid;
	}
	return 0;
}

inline std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}
inline bool IsFloatingType(int typeId)
{
	return typeId == asTYPEID_FLOAT || typeId == asTYPEID_DOUBLE;
}
inline bool isPrimviteType(int typeId)
{
	return typeId == AS_TYPEID_DATETIME || typeId == AS_TYPEID_STRING || typeId <= asTYPEID_DOUBLE || typeId == AS_TYPEID_STRING_T;
}
inline bool isSpecialType(int typeId)
{
	return typeId == AS_TYPEID_ANY || !(typeId & asTYPEID_OBJHANDLE);
}
inline bool isNumericType(int typeId)
{
	return typeId >= asTYPEID_INT8 && typeId <= asTYPEID_DOUBLE;
}
inline std::string AsGetPath(std::string filePath)
{
	std::filesystem::path _path(filePath);
	std::filesystem::path rootPath = "./svencoop";
	std::filesystem::path full = std::filesystem::weakly_canonical(rootPath / _path);
	return full.string();
}
inline std::string AsGenericFormat(asIScriptGeneric* gen, std::string format, int startIndex = 1)
{
	std::string finalResult = "";
	int currentArg = startIndex;
	size_t argc = gen->GetArgCount();
	for (size_t i = 0; i < format.length(); i++) {
		if (format[i] == '%' && i + 1 < format.length()) {

			if (format[i + 1] == '%') {
				finalResult += '%'; i++; continue;
			}
			size_t start = i;
			size_t j = i + 1;
			while (j < format.length() && (isdigit(format[j]) || format[j] == '.' || format[j] == '-' || format[j] == 'l'))
				j++;
			char type = format[j];
			std::string subFormat = format.substr(start, j - start + 1);
			if (currentArg < argc) {
				void* addr = gen->GetArgAddress(currentArg++);
				int typeId = gen->GetArgTypeId(currentArg - 1);
				char part[1024]{ 0 };
				int writed = 0;
				if (type == 'd' || type == 'i' || type == 'u' || type == 'x' || type == 'X') {
					if (typeId == asTYPEID_INT64 || typeId == asTYPEID_UINT64)
						writed = snprintf(part, sizeof(part), subFormat.c_str(), *(int64_t*)addr);
					else if (typeId == asTYPEID_DOUBLE)
						writed = snprintf(part, sizeof(part), subFormat.c_str(), (int)*(double*)addr);
					else
						writed = snprintf(part, sizeof(part), subFormat.c_str(), *(int*)addr);

				}
				else if (type == 'f' || type == 'g' || type == 'e') {
					if (typeId == asTYPEID_DOUBLE)
						writed = snprintf(part, sizeof(part), subFormat.c_str(), *(double*)addr);
					else
						writed = snprintf(part, sizeof(part), subFormat.c_str(), (double)*(float*)addr);
				}
				else if (type == 's') {
					CString* s = (CString*)addr;
					writed = snprintf(part, sizeof(part), subFormat.c_str(), s ? s->c_str() : "");
					i++;
				}
				if (writed > 0)
					finalResult += part;
				i = j;
				continue;
			}
		}
		finalResult += format[i];
	}
	return finalResult;
}
inline void* GetNumericAsVoid(asIScriptGeneric* gen, int index)
{
	if (!gen || !index)
		return nullptr;

	void* addr = gen->GetArgAddress(index);
	if (!addr)
		return nullptr;

	int typeId = gen->GetArgTypeId(index);
	
	if (typeId == asTYPEID_BOOL || typeId == asTYPEID_INT8)
		return (void*)*(asINT8*)addr;
	if (typeId == asTYPEID_UINT8)
		return (void*)*(unsigned char*)addr;
	if (typeId == typeId == asTYPEID_INT16)
		return (void*)*(asINT16*)addr;
	if (typeId == typeId == asTYPEID_UINT16)
		return (void*)*(unsigned short*)addr;
	if (typeId == asTYPEID_INT32)
		return (void*)*(asINT32*)addr;
	if (typeId == asTYPEID_UINT32)
		return (void*)*(asUINT*)addr;
	if (typeId == asTYPEID_INT64)
		return (void*)*(asINT64*)addr;
	if (typeId == asTYPEID_UINT64)
		return (void*)*(unsigned long long*)addr;
	if (typeId == asTYPEID_FLOAT)
	{
		void* arg = nullptr;
		float flVal = *(float*)addr;
		memcpy(&arg, &flVal, sizeof(float));
		return arg;
	}

	if (typeId == asTYPEID_DOUBLE)
		return (void*)new double(*(double*)addr);
	return nullptr;

}

inline void* Array_At(CScriptArray* array, asUINT index)
{
	auto size = array->size();
	if (size <= 0)
		return nullptr;
	auto elementsize = array->m_unk5;
	if (array->m_buffer == 0 || index >= array->m_buffer->size())
	{
		// If this is called from a script we raise a script exception
		//asIScriptContext* ctx = asGetActiveContext();
		//if (ctx)
			//ctx->SetException("Index out of bounds");
		return 0;
	}

	if ((array->m_ElementTypeId & asTYPEID_MASK_OBJECT) && !(array->m_ElementTypeId & asTYPEID_OBJHANDLE))
		return *(void**)(array->m_buffer->m_buf + elementsize * index);
	else
		return array->m_buffer->m_buf + elementsize * index;
}
inline cvar_t* RegisterSVCvar(const char* cvar_name, float cvar_value)
{
	cvar_t _cvar;
	cvar_t& cvar = _cvar;
	cvar.name = new char[25];
	cvar.value = cvar_value;
	cvar.string = new char[10];
	cvar.flags = FCVAR_SERVER;
	sprintf(cvar.string, "%f", cvar_value);
	strcpy(cvar.name, cvar_name);
	g_engfuncs.pfnCVarRegister(&cvar);

	return g_engfuncs.pfnCVarGetPointer(cvar_name);
}

inline int FixOffset(int offset, int linuxOffset)
{
#if defined(__linux__)
	return offset + linuxOffset;
#elif defined(__APPLE__)
	return offset + 3 * 4;
#else
	return offset;
#endif
}
template <typename T>
inline bool PDataSet(edict_t* ent, int offset, T value, int linuxDiff)
{
	if (!ent || !ent->pvPrivateData)
		return false;
	offset = FixOffset(offset, linuxDiff);
	std::memcpy(
		(char*)ent->pvPrivateData + offset,
		&value,
		sizeof(T)
	);
	return true;
}
template <typename T>
inline bool PDataSetPev(entvars_t* pev, int offset, T value, int linuxDiff)
{
	if (!pev) return false;
	edict_t* ent = ENT(pev);
	return PDataSet<T>(ent, offset, value, linuxDiff);
}

template <typename T>
inline bool PDataSetEnt(void* baseEntity, int offset, const T& value, int linuxDiff)
{
	entvars_t* pev = *((entvars_t**)((char*)baseEntity + 4));
	return PDataSetPev<T>(pev, offset, value, linuxDiff);
}

template <typename T>
inline bool PDataGet(edict_t* ent, int offset, T& outValue, int linuxDiff)
{
	if (!ent || !ent->pvPrivateData)
		return false;
	offset = FixOffset(offset, linuxDiff);
	std::memcpy(
		&outValue,
		(char*)ent->pvPrivateData + offset,
		sizeof(T)
	);
	return true;
}

template <typename T>
inline bool PDataGetPev(entvars_t* pev, int offset, T& outValue, int linuxDiff)
{
	if (!pev)
		return false;
	edict_t* ent = ENT(pev);
	return PDataGet<T>(ent, offset, outValue, linuxDiff);
}

template <typename T>
inline bool PDataGetEnt(void* pThis, int offset, T& outValue, int linuxDiff)
{
	if (!pThis)
		return false;
	entvars_t* pev = *((entvars_t**)((char*)pThis + 4));
	return PDataGetPev(pev, offset, outValue, linuxDiff);
}