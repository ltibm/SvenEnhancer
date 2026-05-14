CTextMenu@ slotMenu;
CTextMenu@ slotDetails;
CTextMenu@ slotDetailsWeapons;
dictionary weaponSlots;
dictionary positions;
Cvar@ cvar_fixWeaponPosition;
Cvar@ cvar_maxSlots;
class WeaponSlotItem
{
	int Slot;
	int Position;
	int PositionOriginal;
	bool NotFix  = false;
	WeaponSlotItem(){}
	WeaponSlotItem(int slot, int pos)
	{
		Slot = slot;
		Position = pos;
	}
}
uint64 PackNum(uint left, uint right)
{
    return (int64(left) << 32) | (int64(right) & 0xFFFFFFFF);
}

uint GetLeftInt(int64 packed)
{
    return uint(packed >> 32);
}
uint GetRightInt(int64 packed)
{
    return uint(packed & 0xFFFFFFFF);
}
void PluginInit()
{
	g_Module.ScriptInfo.SetAuthor( "S!" );
	g_Module.ScriptInfo.SetContactInfo( "S" );
	SE_EVENT.OnGame("message.76", @HookWeaponList);
	g_Hooks.RegisterHook(Hooks::Player::ClientSay, @ClientSay);
	@cvar_fixWeaponPosition = SE.RegisterCvar("se_weaponposition_fix", "1", FCVAR_NONE, 1.0f);
	@cvar_maxSlots = SE.RegisterCvar("se_weaponposition_maxpos", "20", FCVAR_NONE, 1.0f);
}
void PluginExit()
{
	if (@slotMenu !is null)
	{
		if(slotMenu.IsRegistered())
			slotMenu.Unregister();
		@slotMenu = null;
	}
	if (@slotDetails !is null)
	{
		if(slotDetails.IsRegistered())
			slotDetails.Unregister();
		@slotDetails = null;
	}	
	if (@slotDetailsWeapons !is null)
	{
		if(slotDetailsWeapons.IsRegistered())
			slotDetailsWeapons.Unregister();
		@slotDetailsWeapons = null;
	}	
}
HookReturnCode ClientSay(SayParameters@ sayitem)
{
    CBasePlayer@ pPlayer = sayitem.GetPlayer();
    if (pPlayer is null)
        return HOOK_CONTINUE;
	const CCommand@ args = sayitem.GetArguments();
	string arg = args[0];
    if (arg == "!slots")
    {
		sayitem.ShouldHide = true;
		ShowSlotMenu(@pPlayer);
        return HOOK_HANDLED;
    }
    return HOOK_CONTINUE;
}
void MenuCallback(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)
{
    if (pItem is null || pPlayer is null){
		menu.Unregister();
		return;
	}
	uint slot;
	pItem.m_pUserData.retrieve(slot);
	ShowSlotDetailMenu(pPlayer, slot);
	menu.Unregister();
}
void MenuCallbackDetail(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)
{
    if (pItem is null || pPlayer is null){
		menu.Unregister();
		if(pPlayer !is null)
		{
			ShowSlotMenu(pPlayer);
		}
		return;
	}
	menu.Unregister();
	uint64 position = 0;
	if(!pItem.m_pUserData.retrieve(position) || position == 999999){
		ShowSlotMenu(pPlayer);
		return;
	}
	ShowSlotDetailWeaponsMenu(pPlayer, position);
}
void MenuCallbackDetailWeapons(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)
{
    if (pItem is null || pPlayer is null){
		menu.Unregister();
		if(pPlayer !is null)
		{
			const CTextMenuItem@ _it = menu.GetItem(0);
			if(_it !is null)
			{
				uint64 position;
				_it.m_pUserData.retrieve(position);
				uint _slot = GetLeftInt(position);
				ShowSlotDetailMenu(pPlayer, _slot);
			}
			else
			{
				ShowSlotMenu(pPlayer);
			}
		}
		return;
	}
	uint64 position;
	pItem.m_pUserData.retrieve(position);
	uint _slot = GetLeftInt(position);
	ShowSlotDetailMenu(pPlayer, _slot);
	menu.Unregister();
}

void ShowSlotDetailWeaponsMenu(CBasePlayer@ pPlayer, uint64 slotPos)
{
	@slotDetailsWeapons = CTextMenu(@MenuCallbackDetailWeapons);
	uint _slot = GetLeftInt(slotPos);
	uint _position = GetRightInt(slotPos);
	slotDetailsWeapons.SetTitle("Slot: " + _slot + " Pos: " + _position + " ");
	array<string> keys = weaponSlots.getKeys();
	for(uint i = 0; i < keys.length(); i++)
	{
		WeaponSlotItem item;
		if(weaponSlots.get(keys[i], item) && item.PositionOriginal == _position && item.Slot ==  _slot)
		{
			slotDetailsWeapons.AddItem(keys[i] + "" + (item.Position != item.PositionOriginal ? ("(fixed " + item.Position + ")" ) : "") + "" + (item.NotFix ? "(not fixed)" : "") , any(slotPos));
		}
	}
	slotDetailsWeapons.Register();
    slotDetailsWeapons.Open(0, 0, pPlayer);
	
}
void ShowSlotDetailMenu(CBasePlayer@ pPlayer, uint slot)
{
	array<string> keys = weaponSlots.getKeys();
	dictionary usedCounts;
	array<uint> _newPos;
	for(uint i = 0; i < keys.length(); i++)
	{
		WeaponSlotItem item;
		if(weaponSlots.get(keys[i], item) && item.Slot == slot)
		{
			uint curUsed = 0;
			if(usedCounts.get(item.PositionOriginal, curUsed))
			{
				if(curUsed > 0)
				{
					usedCounts[item.PositionOriginal] = curUsed + 1;
					continue;
				}
			}
			else
			{
				usedCounts[item.PositionOriginal] = 1;
			}
			_newPos.insertLast(item.PositionOriginal);
		}
	}
	_newPos.sortAsc();
    @slotDetails = CTextMenu(@MenuCallbackDetail);

    slotDetails.SetTitle("Slot(Original) " + slot + " ");
	for(uint i = 0; i < _newPos.length(); i++)
	{
		uint used = 0;
		usedCounts.get(_newPos[i], used);
		slotDetails.AddItem("Position " + _newPos[i] + " (\y" + used + "\w)", any(PackNum(slot, _newPos[i])));
	}
	if(_newPos.length() == 0)
	{
		slotDetails.AddItem("No Weapons Found",  any(uint64(999999)));
	}
    slotDetails.Register();
    slotDetails.Open(0, 0, pPlayer);
}
void ShowSlotMenu(CBasePlayer@ pPlayer)
{
    @slotMenu = CTextMenu(@MenuCallback);
    slotMenu.SetTitle("Weapon Slots ");
    for (int i = 0; i <= 9; i++)
    {
        slotMenu.AddItem("Slot " + i, any(i));
    }
    slotMenu.Register();
    slotMenu.Open(0, 0, pPlayer);
}


void MapInit()
{
	weaponSlots.clear();
	positions.clear();
}
void GetSlotUsages()
{
	
}
bool PosAvailable(uint slot, uint pos)
{
	string posKey = PackNum(slot, pos);
	uint numUsed = 0;
	bool _status = true;
	if(positions.get(posKey, numUsed))
	{
		_status = numUsed <= 0;
	}
	else
		numUsed = 0;
	return _status;
}
void IncreaseUsed(uint slot, uint pos)
{
	uint numUsed = 0;
	string posKey = PackNum(slot, pos);
	if(!positions.get(posKey, numUsed))
	{
		numUsed = 0;
	}
	positions[posKey] = ++numUsed;
}
int GetAvailablePos(int slot, int currentPos)
{
	int _loopCount = 0;
	int cvarVal = cvar_maxSlots.value;
	if(cvarVal <= 0)
		return currentPos;
	while(true)
	{
		_loopCount++;
		uint newPos = (currentPos + _loopCount) % (cvarVal + 1);
		if(newPos == currentPos || _loopCount >= 1000)
			return currentPos;
		if(PosAvailable(slot, newPos))
			return newPos;
	} 
	return 0;
}
void HookWeaponList(CallbackItem@ item)
{
	if(SE_EVENT.MessageContext.GetArgType(0) != SEMSG_STRING)
		return;
	string className = SE_EVENT.MessageContext.GetArgString(0);
	int iSlot = SE_EVENT.MessageContext.GetArgByte(5);
	int iPosition = SE_EVENT.MessageContext.GetArgByte(6);
	WeaponSlotItem slotItem;
	if(weaponSlots.get(className, slotItem))
	{
		SE_EVENT.MessageContext.SetArgByte(6, slotItem.Position);
		return;
	}
	slotItem.Slot = iSlot;
	slotItem.PositionOriginal = iPosition;
	if(PosAvailable(iSlot, iPosition))
	{
		slotItem.Position = iPosition;
	}
	else
	{
		if(cvar_fixWeaponPosition.value > 0)
		{
			slotItem.Position = GetAvailablePos(iSlot, iPosition);
			if(slotItem.Position == slotItem.PositionOriginal)
			{
				//No empty slots founds
				slotItem.NotFix = true;
			}
			else
			{
				//Increase original position used counts
				IncreaseUsed(iSlot, slotItem.PositionOriginal);
			}
			SE_EVENT.MessageContext.SetArgByte(6, slotItem.Position);
		}
		else
		{
			slotItem.Position = iPosition;
		}
	}
	IncreaseUsed(iSlot, slotItem.Position);
	weaponSlots[className] = slotItem;

}