# Voice Management
With SvenEnhancer, you can now perform partial, full, and team-based voice chat blocking.

# SEVoiceState class
```angelscript
  //Others player
  //If set ture cannot be heard by other players.
  bool Muted;
  //Only players with the same TeamId can hear each other.
  int TeamId;
  //This is automaticly filled by admins.txt but you can change them
  //If SE.Voice_AdminOnly is set to true, voice chat can only be used by players whose IsAdmin value is true.
  bool IsAdmin;
  //If the Level value is greater than SE.Voice_MinLevel, other players can hear them.
  int Level;
```

# Usage
```angelscript

  //Player voicestate automaticly reset when player disconnecting, no more reset required

  //Sample playerId or edict_t@
  int playerId = 1;
  SEVoiceState@ voiceState = SE.GetVoiceState(playerId);

  //Admin only voice
  SE.Voice_AdminOnly = true;

  //Disable voice
  SE.Voice_Disabled = true;

  //Set mininum level requirement for voice
  SE.Voice_MinLevel = 1;
  
  if(voiceState !is null)
  {
    //Automaticly filled by module, but its changeable, check SE.Voice_AdminOnly
    voiceState.IsAdmin = true;
    voiceState.Level = 1;
    //Only players with TeamId 3 can hear each other.
    voiceState.TeamId = 3;

    //check SE.Voice_MinLevel
    voiceState.Level = 1;
    
  }
```
