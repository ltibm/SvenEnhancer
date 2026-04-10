#  EntitySupport SvenEnhancerEntity(SE_ENT)
You can access entity methods directly via SE_ENT.
Here is supported methods;
```angelscript
bool SetPDataInt(CBaseEntity@ ent, int offset, int value, int linuxdiff = 12);
int GetPDataInt(CBaseEntity@ ent, int offset, int linuxdiff = 12);
bool SetPDataUInt(CBaseEntity@ ent, int offset, uint value, int linuxdiff = 12);
uint GetPDataUInt(CBaseEntity@ ent, int offset, int linuxdiff = 12);
bool SetPDataBool(CBaseEntity@ ent, int offset, bool value, int linuxdiff = 12);
bool GetPDataBool(CBaseEntity@ ent, int offset, int linuxdiff = 12);
bool SetPDataFloat(CBaseEntity@ ent, int offset, float value, int linuxdiff = 12);
float GetPDataFloat(CBaseEntity@ ent, int offset, int linuxdiff = 12);
bool SetPDataShort(CBaseEntity@ ent, int offset, int16 value, int linuxdiff = 12);
int16 GetPDataShort(CBaseEntity@ ent, int offset, int linuxdiff = 12);
bool SetPDataUShort(CBaseEntity@ ent, int offset, uint16 value, int linuxdiff = 12);
uint16 GetPDataUShort(CBaseEntity@ ent, int offset, int linuxdiff = 12);

//Allow to modify entity or player global data
dictionary@+ GetData(CBaseEntity@ entity);
dictionary@+ GetData(edict_t@ edict);
dictionary@+ GetData(int index);
```

# Sample Usage

```angelscript
CBaseEntity@ entity = GetEntity(12345);
dictionary@ data = SE_ENT.GetData(@entity);

//All plugins can access this data.
data["test"] = "test";

//Set Entity Private Data Offset, An incorrect assignment may result in a crash.
SE_ENT.SetPDataInt(@entity, 12345, 10);

//The Linux offset difference has been set to 12 by default and can be optionally changed.
//No action is required for Windows.
SE_ENT.SetPDataInt(@entity, 12345, 10, 12);


//Reading PDATA
int pdata = SE_ENT.SetPDataInt(@entity, 12345);
```


