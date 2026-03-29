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
	~CallbackItem();
	static CallbackItem* Factory();
private:
	int m_iMode = 0;
};
void RegisterCallBackItem(CASDocumentation* pASDoc, asIScriptEngine* engine);