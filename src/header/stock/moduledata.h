#pragma once;
#include <svenenhancer.h>

class ModuleData {
public:
	std::string name;
	std::string moduleName;
	asIScriptModule* module;
	bool ready;
	std::unordered_map<std::string, std::unordered_map<int, asIScriptFunction*>*> scriptCallbacks;
	std::unordered_map<std::string, asIScriptFunction*> methodCallbacks;
	void Clear();
};

class ModuleReadyData {
public:
	std::vector<std::string> moduleNames;
	asIScriptFunction* function;
};
extern std::vector<ModuleData*> g_ModuleData;

extern std::vector<ModuleReadyData*> g_ModuleReadyData;


struct StructModuleName {
	std::string module;
	std::string name;
	static StructModuleName fromStringDir(std::string v);
	static StructModuleName fromString(std::string v);
};

ModuleData* findModuleByModuleName(char* module);
ModuleData* findModuleByModule(asIScriptModule* module);
ModuleData* findModuleByName(std::string name);   

void deleteModuleData(asIScriptModule* data);