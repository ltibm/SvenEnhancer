#pragma once

#define SE_VERSION_FLOAT 0.70
#define SE_VERSION "0.70"
#define SE_DATE "08.04.2026"
#define SE_PRINT(msg) \
    g_engfuncs.pfnServerPrint("[Sven Enhancer] " msg "\r\n")

extern int AS_TYPEID_STRING;
extern int AS_TYPEID_STRING_T;
extern int AS_TYPEID_ARRAY;
extern int AS_TYPEID_DICTIONARY;
extern int AS_TYPEID_DATETIME;
extern int AS_TYPEID_ANY;
extern int AS_TYPEID_ARRAY_ANY;
extern int AS_TYPEID_ANY_HANDLE;
extern int AS_TYPEID_ARRAY_ANY_HANDLE;

void InitializeGlobalTypes(asIScriptEngine* engine);

#define REGISTE_OBJMETHODEX(r, d, e, c, m, cc, mm, call) r=asMETHOD(cc,mm);ASEXT_RegisterObjectMethodEx(d,e,c,m,&r,call)
#define REGISTE_OBJMETHODPREX(r, d, e, c, m, cc, mm, pp, rr, call) r=asMETHODPR(cc,mm, pp, rr);ASEXT_RegisterObjectMethodEx(d,e,c,m,&r,call)



template <typename T>
void RegisteRefObject(CASDocumentation* pASDoc, const char* szName) {
	asSFuncPtr reg;
	reg = asMETHOD(T, AddRef);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "AddRef", szName, asBEHAVE_ADDREF, "void AddRef()", &reg, asCALL_THISCALL);
	reg = asMETHOD(T, Release);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Release", szName, asBEHAVE_RELEASE, "void Release()", &reg, asCALL_THISCALL);
}
template <typename T>
void RegisteGCObject(CASDocumentation* pASDoc, const char* szName) {
	RegisteRefObject<T>(pASDoc, szName);
	asSFuncPtr reg;
	reg = asMETHOD(T, SetGCFlag);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Set GC Flag", szName, asBEHAVE_SETGCFLAG, "void SetGCFlag()", &reg, asCALL_THISCALL);
	reg = asMETHOD(T, GetGCFlag);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Get GC Flag", szName, asBEHAVE_GETGCFLAG, "bool GetGCFlag() const", &reg, asCALL_THISCALL);
	reg = asMETHOD(T, GetRefCount);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Get reference count", szName, asBEHAVE_GETREFCOUNT, "int GetRefCount() const", &reg, asCALL_THISCALL);
	reg = asMETHOD(T, EnumReferences);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Enumerate references held by this class", szName, asBEHAVE_ENUMREFS, "void EnumReferences(int& in)", &reg, asCALL_THISCALL);
	reg = asMETHOD(T, ReleaseReferences);
	ASEXT_RegisterObjectBehaviourEx(pASDoc, "Release all references held by this class", szName, asBEHAVE_RELEASEREFS, "void ReleaseReferences(int& in)", &reg, asCALL_THISCALL);
}


template <typename T>
int RegsiterObject(const char* name, asIScriptEngine* engine, int type =-1)
{
	int r = 0;
	r = engine->RegisterObjectType(name, sizeof(T), type == -1 ? asOBJ_REF | asOBJ_GC : type);
	r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void AddRef()", asMETHOD(T, AddRef), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void Release()", asMETHOD(T, Release), asCALL_THISCALL);


	r = engine->RegisterObjectBehaviour(name, asBEHAVE_SETGCFLAG, "void SetGCFlag()", asMETHOD(T, SetGCFlag), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETGCFLAG, "bool GetGCFlag() const", asMETHOD(T, SetGCFlag), asCALL_THISCALL);


	r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETREFCOUNT, "int GetRefCount() const", asMETHOD(T, GetRefCount), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour(name, asBEHAVE_ENUMREFS, "void EnumReferences(int& in)", asMETHOD(T, EnumReferences), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASEREFS, "void ReleaseReferences(int& in)", asMETHOD(T, ReleaseReferences), asCALL_THISCALL);
	return r;
}


template<typename T>
T GetSymbol(void* handle, const char* name)
{
#ifdef _WIN32
	return reinterpret_cast<T>(GetProcAddress((HMODULE)handle, name));
#else
	return reinterpret_cast<T>(dlsym(handle, name));
#endif
}