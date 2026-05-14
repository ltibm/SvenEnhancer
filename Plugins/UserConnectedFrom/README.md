# Description

Show Connection User Country To All Users

Say Commands To Show Users Country: **!wherefrom**

-- Only admin can see users Isp and IpAdress, others users only can see Country

Server Command To Show Users Country: **wherefrom**

# Cvar

**user_connected_dest** (0 to 2): 0 Disabled, 1 Show message on Client Connect, 2 Show message on Client Put in server

**user_connected_format** (default: "{nick} Connecting from [{country}]"): you can edit connectig from message

# Requirements

SvenEnhancer

# Installation

append to default_plugins.txt
```
     "plugin"
    {
    	"name" "User Connected From"
    	"script" "user_connectedfrom"
    }
```
