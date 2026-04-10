#pragma once
inline asIScriptEngine* GetASEngine() {
	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptEngine* engine = manager->scriptEngine;
	return engine;
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