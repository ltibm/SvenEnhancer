#pragma once
class JValue;

class SvenEnhancerAs  : public CASBaseGCObject
{
public:
	SvenEnhancerAs();
	~SvenEnhancerAs();
	JValue* Json_ParseFromFile(CString& input);
	JValue* Json_Parse(CString& input);
	JValue* Json_ParseObject(void* obj, asITypeInfo* typeId);
	JValue* Json_ParseObjectV2(void* obj, int typeId);
	CString* Interpolate(CString& input, void* dict);
	CString* Version();
	RefObject* pDictionary = nullptr;
	void* getGlobals();
private:
	void init();
};

