#pragma once
inline asIScriptEngine* GetASEngine() {
	CASServerManager* manager = ASEXT_GetServerManager();
	asIScriptEngine* engine = manager->scriptEngine;
	return engine;
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
