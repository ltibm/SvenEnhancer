#include <svenenhancer.h>
int AS_TYPEID_STRING = 0;
int AS_TYPEID_STRING_T = 0;
int AS_TYPEID_ARRAY = 0;
int AS_TYPEID_DICTIONARY = 0;
int AS_TYPEID_DATETIME = 0;
int AS_TYPEID_ANY = 0;
int AS_TYPEID_ARRAY_ANY = 0;
int AS_TYPEID_ANY_HANDLE = 0;
int AS_TYPEID_ARRAY_ANY_HANDLE = 0;


void InitializeGlobalTypes(asIScriptEngine* engine)
{
	AS_TYPEID_STRING = AS_GetTypeId(engine, "string");
	AS_TYPEID_STRING_T = AS_GetTypeId(engine, "string_t");
	AS_TYPEID_DICTIONARY = AS_GetTypeId(engine, "dictionary");
	AS_TYPEID_ARRAY = AS_GetTypeId(engine, "array");
	AS_TYPEID_DATETIME = AS_GetTypeId(engine, "DateTime");
	AS_TYPEID_ANY = AS_GetTypeId(engine, "any");
	AS_TYPEID_ANY_HANDLE = AS_TYPEID_ANY | asTYPEID_OBJHANDLE;
	AS_TYPEID_ARRAY_ANY = AS_GetTypeId(engine, "array<any>", true);
	AS_TYPEID_ARRAY_ANY_HANDLE = AS_GetTypeId(engine, "array<any@>", true);

}