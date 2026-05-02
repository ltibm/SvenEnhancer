#pragma once
#include <vector>
#include <unordered_map>
#include <svenenhancer.h>
#define CHECK_INDEX(index) if (index < 0 || index >= (int)params.size()) return;
class MessageContext
{
public:
    bool hooked;
    int msgType;
	int msgDest;
    Vector origin;
    bool blocked = false;
    struct Param
    {
        enum Type { Byte, Char, Short, Long, Angle, Coord, String, Entity };
        Type type;
        int i;
        float f;
        char* str;
        Vector v;
		bool modified = false;
    };
    edict_t* edict;
    std::vector<Param> params;
    ~MessageContext() {
        Clear();
    }
	int GetArgCount() {
		return params.size();
	}
	int GetArgType(int index) {
		if (params.size() > index)
			return params[index].type;
		return -1;
    };
    asBYTE GetArgByte(int index) {
        if (params.size() > index)
            return params[index].i;
        return 0;
    }
    void SetArgByte(int index, asBYTE value) {
        CHECK_INDEX(index);
        params[index].i = value;
        params[index].modified = true;
    }
	int AddArg(int type) {
		if (type < Param::Byte || type > Param::Entity)
			return -1;
		Param p;
		p.type = static_cast<Param::Type>(type);
		params.push_back(p);
		return params.size() - 1;
	}
    float ReadFloat(int index)
    {
        if (index < 0 || index + 3 >= (int)params.size())
            return 0.0f;

        uint32_t raw =
            (uint32_t)(uint8_t)params[index].i |
            ((uint32_t)(uint8_t)params[index + 1].i << 8) |
            ((uint32_t)(uint8_t)params[index + 2].i << 16) |
            ((uint32_t)(uint8_t)params[index + 3].i << 24);

        float value;
        std::memcpy(&value, &raw, sizeof(float));
        return value;
    }
	void SetFloat(int index, float value)
	{
		if (index < 0 || index + 3 >= (int)params.size())
			return;
		int maxi = index + 3;
        while (params.size() < maxi)
        {
            Param p;
			p.type = Param::Byte;
			params.push_back(p);
        }
		uint32_t raw;
		std::memcpy(&raw, &value, sizeof(float));
		params[index + 0].i = raw & 0xFF;
		params[index + 0].type = Param::Byte;
		params[index + 0].modified = true;
		params[index + 1].i = (raw >> 8) & 0xFF;
        params[index + 1].type = Param::Byte;
        params[index + 1].modified = true;
		params[index + 2].i = (raw >> 16) & 0xFF;
        params[index + 2].type = Param::Byte;
        params[index + 2].modified = true;
		params[index + 3].i = (raw >> 24) & 0xFF;
        params[index + 3].type = Param::Byte;
		params[index + 3].modified = true;
	}
    asINT16 GetArgShort(int index) {
        if (params.size() > index)
            return params[index].i;
        return 0;
    }
    void SetArgShort(int index, asINT16 value) {
        CHECK_INDEX(index);
        params[index].i = value;
        params[index].modified = true;
    }
    asINT32 GetArgInt(int index) {
        if (params.size() > index)
            return params[index].i;
        return 0;
    }
    void SetArgInt(int index, asINT32 value) {
        CHECK_INDEX(index);
        params[index].i = value;
        params[index].modified = true;
    }
	edict_t* GetArgEntity(int index) {
		if (params.size() > index)
			return INDEXENT(params[index].i);
		return nullptr;
	}
	int GetArgEntityI(int index) {
		if (params.size() > index)
			return params[index].i;
		return -1;
	}
	void SetArgEntity(int index, edict_t* value) {
        CHECK_INDEX(index);
        params[index].i = ENTINDEX(value);
        params[index].modified = true;
	}
	void SetArgEntityI(int index, int value) {
        CHECK_INDEX(index);
        params[index].i = value;
		params[index].modified = true;
	}

	float GetArgFloat(int index) {
		if (params.size() > index)
			return params[index].f;
		return 0.0f;
	}
	void SetArgFloat(int index, float value) {
		CHECK_INDEX(index);
        params[index].f = value;
        params[index].modified = true;
	}

    CString* GetArgString(int index, bool clean) {
        std::string _str;
        CString* str = new CString();
        if (params.size() > index && params[index].str)
            _str.assign(params[index].str, strlen(params[index].str));
        if (clean && !_str.empty())
        {
            if ((uint8_t)_str[0] < 0x20)
                _str.erase(0, 1);
            if (!_str.empty() && _str.back() == '\n')
                _str.pop_back();
        }
        str->assign(_str.c_str(), _str.size());
        return str;
    }
	void* SplitArgString(int index, bool clean) {
        std::string _str;
        if (params.size() > index && params[index].str)
            _str.assign(params[index].str, strlen(params[index].str));
        if (clean && !_str.empty())
        {
            if ((uint8_t)_str[0] < 0x20)
                _str.erase(0, 1);
            if (!_str.empty() && _str.back() == '\n')
                _str.pop_back();
        }
        std::vector<std::string> out;
        size_t pos = _str.find(':');
        if (pos == std::string::npos)
        {
            out.push_back(_str);
        }
        else {
            std::string name = _str.substr(0, pos);
            std::string msg = _str.substr(pos + 1);
            if (!msg.empty() && msg[0] == ' ')
                msg.erase(0, 1);
            out.push_back(name);
            out.push_back(msg);
        }
        return CreateASArray_FromVector(out);

	}
    void SetArgString(int index, CString& value, bool clean) {
        if (params.size() <= index)
            return;
        if (params[index].str)
        {
            free(params[index].str);
            params[index].str = nullptr;
        }

		params[index].modified = true;
        std::string finalValue = value.c_str();

        if (clean)
        {
            finalValue = "\x02" + finalValue;
            finalValue += "\n";
        }
        params[index].str = AS_STRDUP(finalValue.c_str());
    }
    void Clear()
    {
        for (auto& param : params)
        {
            if (param.type == Param::String && param.str)
            {
                free(param.str);
            }
        }
        params.clear();
    }
	bool IsModified(int index) {
		if (params.size() > index)
			return params[index].modified;
		return false;
	}
};
extern MessageContext* msgContext;
void Hook_MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);
void Hook_Messageend();
void Hook_WriteByte(int iValue);
void Hook_WriteChar(int iValue);
void Hook_WriteShort(int iValue);
void Hook_WriteLong(int iValue);
void Hook_WriteAngle(float flValue);
void Hook_WriteCoord(float flValue);
void Hook_WriteString(const char* sz);
void Hook_WriteEntity(int ent);

void RegisterSEMessageContext(CASDocumentation* pASDoc, asIScriptEngine* engine);