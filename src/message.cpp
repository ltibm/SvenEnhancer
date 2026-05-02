#include <svenenhancer.h>
#include "message.h"
MessageContext* msgContext = NULL;
bool msgBlocked = false;
#define CHECK_BLOCKED() \
    do { \
        if(msgBlocked) \
            RETURN_META(MRES_SUPERCEDE); \
    } while(0)
#define HOOKED() (msgContext && msgContext->hooked)
void Hook_MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	CHECK_BLOCKED();
	auto key = MAKE_EVENTKEY(EVENT_MESSAGE, msg_type);
	auto event = g_SEEvent.g_gameEvents.find(key);
	if (event == g_SEEvent.g_gameEvents.end())
	{
		RETURN_META(MRES_IGNORED);
	}
	bool _hooked = false;
	msgContext = new MessageContext();
	msgContext->hooked = true;
	msgContext->msgType = msg_type;
	msgContext->edict = ed;
	msgContext->msgDest = msg_dest;
	if (pOrigin)
		msgContext->origin = Vector(pOrigin[0], pOrigin[1], pOrigin[2]);
	RETURN_META(MRES_SUPERCEDE);


}

void Hook_Messageend()
{
	if (msgBlocked)
	{
		msgBlocked = false;
		RETURN_META(MRES_SUPERCEDE);
	}
	if (msgContext)
	{
		if (msgContext->hooked)
		{

			CallbackItem* item = new CallbackItem();
			auto key = MAKE_EVENTKEY(EVENT_MESSAGE, msgContext->msgType);
			g_SEEvent.TriggerGameEvent(key, item);
			if (msgContext->blocked)
			{
				delete item;
				item = nullptr;
				RETURN_META(MRES_SUPERCEDE);
			}
		
			float buffer[3];
			msgContext->origin.CopyToArray(buffer);
			MESSAGE_BEGIN(msgContext->msgDest, msgContext->msgType, buffer, msgContext->edict);
			for (auto& p : msgContext->params)
			{
				switch (p.type)
				{
				case MessageContext::Param::Byte:
					WRITE_BYTE(p.i);
					break;
				case MessageContext::Param::Char:
					WRITE_CHAR(p.i);
					break;
				case MessageContext::Param::Short:
					WRITE_SHORT(p.i);
					break;
				case MessageContext::Param::Long:
					WRITE_LONG(p.i);
					break;
				case MessageContext::Param::Angle:
					WRITE_ANGLE(p.f);
					break;
				case MessageContext::Param::Coord:
					WRITE_COORD(p.f);
					break;
					break;
				case MessageContext::Param::String:
					WRITE_STRING(p.str);
					break;
				case MessageContext::Param::Entity:
					WRITE_ENTITY(p.i);
					break;
				default:
					break;
				}
			}
			MESSAGE_END();
			delete item;

		}
		delete msgContext;
		msgContext = nullptr;
		RETURN_META(MRES_SUPERCEDE);

	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteByte(int iValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Byte;
		p.i = iValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteChar(int iValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Char;
		p.i = iValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteShort(int iValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Short;
		p.i = iValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteLong(int iValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Long;
		p.i = iValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteAngle(float flValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Angle;
		p.f = flValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteCoord(float flValue)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Coord;
		p.f = flValue;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteString(const char* sz)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::String;
		p.str = AS_STRDUP(sz);
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}

void Hook_WriteEntity(int ent)
{
	CHECK_BLOCKED();
	if (HOOKED())
	{
		MessageContext::Param p;
		p.type = MessageContext::Param::Entity;
		p.i = ent;
		msgContext->params.push_back(p);
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}
static MessageContext* GetMessageContext(void* pThis)
{
	return msgContext;
}

void RegisterSEMessageContext(CASDocumentation* pASDoc, asIScriptEngine* engine)
{
	engine->RegisterEnum("SEMessageArgType");
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_BYTE", MessageContext::Param::Byte);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_CHAR", MessageContext::Param::Char);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_SHORT", MessageContext::Param::Short);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_LONG", MessageContext::Param::Long);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_ANGLE", MessageContext::Param::Angle);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_COORD", MessageContext::Param::Coord);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_STRING", MessageContext::Param::String);
	engine->RegisterEnumValue("SEMessageArgType", "SEMSG_ENTITY", MessageContext::Param::Entity);

	engine->RegisterObjectType("SEMessageContext", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("SEMessageContext", "int MsgType", offsetof(MessageContext, msgType), asCALL_THISCALL);
	engine->RegisterObjectProperty("SEMessageContext", "int MsgDest", offsetof(MessageContext, msgDest), asCALL_THISCALL);
	engine->RegisterObjectProperty("SEMessageContext", "Vector Origin", offsetof(MessageContext, origin), asCALL_THISCALL);
	engine->RegisterObjectProperty("SEMessageContext", "bool Blocked", offsetof(MessageContext, blocked), asCALL_THISCALL);
	engine->RegisterObjectProperty("SEMessageContext", "edict_t@ Edict", offsetof(MessageContext, edict), asCALL_THISCALL);

	engine->RegisterObjectMethod("SEMessageContext", "string& GetArgString(int index, bool clean=true) const", asMETHOD(MessageContext, GetArgString), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "array<string>@ SplitArgString(int index, bool clean=true) const", asMETHOD(MessageContext, SplitArgString), asCALL_THISCALL);

	engine->RegisterObjectMethod("SEMessageContext", "int GetArgCount() const", asMETHOD(MessageContext, GetArgCount), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int GetArgType(int index) const", asMETHOD(MessageContext, GetArgType), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int8 GetArgByte(int index) const", asMETHOD(MessageContext, GetArgByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int8 GetArgChar(int index) const", asMETHOD(MessageContext, GetArgByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int16 GetArgShort(int index) const", asMETHOD(MessageContext, GetArgShort), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int GetArgLong(int index) const", asMETHOD(MessageContext, GetArgInt), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "float GetArgFloat(int index) const", asMETHOD(MessageContext, GetArgFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "edict_t@ GetArgEntity(int index) const", asMETHOD(MessageContext, GetArgEntity), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "bool IsArgModified(int index) const", asMETHOD(MessageContext, IsModified), asCALL_THISCALL);

	engine->RegisterObjectMethod("SEMessageContext", "void SetArgByte(int index, int8 value)", asMETHOD(MessageContext, SetArgByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgChar(int index, int8 value)", asMETHOD(MessageContext, SetArgByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgShort(int index, int16 value)", asMETHOD(MessageContext, SetArgShort), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgLong(int index, int value)", asMETHOD(MessageContext, SetArgInt), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgFloat(int index, float value)", asMETHOD(MessageContext, SetArgFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgEntity(int index, edict_t@ value)", asMETHOD(MessageContext, SetArgEntity), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgEntity(int index, int value)", asMETHOD(MessageContext, SetArgEntityI), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetArgString(int index, string&in value, bool clean = true)", asMETHOD(MessageContext, SetArgString), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "int AddArg(SEMessageArgType type)", asMETHOD(MessageContext, AddArg), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "float ReadFloat(int index) const", asMETHOD(MessageContext, ReadFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void SetFloat(int index, float value)", asMETHOD(MessageContext, SetFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("SEMessageContext", "void Clear()", asMETHOD(MessageContext, Clear), asCALL_THISCALL);

	engine->RegisterObjectMethod(
		"SvenEnhancerEvent",
		"SEMessageContext@ get_MessageContext()",
		asFUNCTION(GetMessageContext),
		asCALL_CDECL_OBJFIRST
	);

}
