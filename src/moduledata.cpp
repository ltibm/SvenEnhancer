#include <moduledata.h>
#include <sqlite3_sven.h>
std::vector<ModuleData*> g_ModuleData;
std::vector<ModuleReadyData*> g_ModuleReadyData;
ModuleData* findModuleByModuleName(char* module)
{
	for (const auto& key : g_ModuleData) {

		if (key->moduleName == module)
			return key;
	}
	return nullptr;
}
ModuleData* findModuleByModule(asIScriptModule* module)
{
	for (const auto& key : g_ModuleData) {

		if (key->module == module)
			return key;
	}
	return nullptr;
}
ModuleData* findModuleByName(std::string name)
{
	for (const auto& key : g_ModuleData) {

		if (key->name == name)
			return key;
	}
	return nullptr;
}

void deleteModuleData(asIScriptModule* module)
{
	SqliteConnection::CloseAllConnectionsByModule(module);
	//Remove all register clientcmds
	g_SE->UnregisterClientCmdsByModule(module);
	//Remove all register servercmds
	g_SE->UnregisterServerCmdsByModule(module);
	g_SEEvent.ClearEventByModule(module);
	for (const auto& key : g_ModuleData) {

		if (key->module == module)
			key->Clear();
			g_ModuleData.erase(std::remove(g_ModuleData.begin(), g_ModuleData.end(), key), g_ModuleData.end());
			delete key;
			break;
	}
	std::erase_if(g_ModuleReadyData, [module](ModuleReadyData* data) {
		if (!data || !data->function)
			return false;
		bool _del = data->function->GetModule() == module;
		if (_del)
		{
			data->function->Release();
		}
		return _del;
	});
	{
		//Remove async request for discarding module
		std::lock_guard<std::recursive_mutex> lock(g_ClientMutex);
		std::queue<InternalRequest*> temp;
		while (!m_Completed.empty())
		{
			InternalRequest* req = m_Completed.front();
			m_Completed.pop();

			bool remove = false;

			if (req->Callback)
			{
				asIScriptModule* mod = req->Callback->GetModule();
				if (mod == module)
					remove = true;
			}
			if (remove)
			{
				if (req->Callback)
				{
					req->Callback->Release();
					req->Callback = nullptr;
				}
				delete req;
			}
			else
			{
				temp.push(req);
			}
		}
		m_Completed = std::move(temp);
	}
}

StructModuleName StructModuleName::fromStringDir(std::string v) {
	StructModuleName n;
	CASServerManager* manager = ASEXT_GetServerManager();
	if (manager->curModule)
	{
		auto module = GetModulePtr(manager);
		if (module)
		{
			auto self = findModuleByModule(module);
			if (self)
			{
				n.module = self->name;
			}
		}
	}
	n.name = v;
	return n;
}
StructModuleName StructModuleName::fromString(std::string v)
{
	StructModuleName n;
	auto vec = split(v, ".");
	if (vec.size() > 1)
	{
		n.module = vec[0];
		n.name = vec[1];
	}
	else if (vec.size() == 1)
	{
		CASServerManager* manager = ASEXT_GetServerManager();
		if (manager->curModule)
		{
			auto module = GetModulePtr(manager);
			if (module)
			{
				auto self = findModuleByModule(module);
				if (self)
				{
					n.module = self->name;
				}
			}
		}

		n.name = vec[1];

	}
	return n;
}

void ModuleData::Clear() {
	this->ready = false;
	this->module = nullptr;
	this->moduleName = "";
	this->name = "";
	for (auto& [key, value] : this->scriptCallbacks)
	{
		delete value;
	}
	for (auto& it : methodCallbacks)
	{
		if (it.second)
			it.second->Release();
	}
	for (auto& it : scriptCallbacks)
	{
		if (it.second)
		{
			for (auto& it2 : *it.second)
			{
				if (it2.second)
					it2.second->Release();
			}
			it.second->clear();
		}
	}
	this->scriptCallbacks.clear();
	this->methodCallbacks.clear();
}