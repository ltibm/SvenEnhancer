#pragma once
class CallbackItem : public CASBaseGCObject
{
public:
	int Id = 0;
	int ReturnCode = 0;
	bool StopCall = false;
	CString* Name;
	asIScriptObject* Data = nullptr;
	asIScriptObject* Dict = nullptr;
	CString ReturnString;
	inline CString* GetModuleName() {
		CString* str = new CString();
		str->assign(moduleName.c_str(), moduleName.size());
		return str;
	}
	CallbackItem();
	~CallbackItem();
	static CallbackItem* Factory();
	std::string moduleName{};
private:
	int m_iMode = 0;
};
void RegisterCallBackItem(CASDocumentation* pASDoc, asIScriptEngine* engine);