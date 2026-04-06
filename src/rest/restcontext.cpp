#include "rest.h"
RestContext* RestContext::Factory()
{
    RestContext* obj = new RestContext();
    CASServerManager* manager = ASEXT_GetServerManager();
    asIScriptEngine* engine = manager->scriptEngine;
    asITypeInfo* type = engine->GetTypeInfoByName("RestContext");
    engine->NotifyGarbageCollectorOfNewObject(obj, type);
    return obj;
}

RestContext::RestContext()
{
}
