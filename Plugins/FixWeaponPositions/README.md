# Description
This plugin automatically fixes the issue where weapons registered to the same slot and position cannot be displayed properly on the client side.

To enable automatic fixing, set the **se_weaponposition_fix cvar** to 1 (default is 1).

You can configure the maximum position count using the **se_weaponposition_maxpos** cvar.

By typing !slots in-game, you can view the current slot positions and detect conflicts. Weapons that have been automatically adjusted will be marked with fixed.

# Requirements
SvenEnhancer

# Installation

append to default_plugins.txt
```
	"plugin"
	{
		"name" "SE Fix Weapon Position"
		"script" "se_fix_weaponposition"
	}
```
