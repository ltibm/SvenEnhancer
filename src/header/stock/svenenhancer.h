#pragma once
#include <string>
#include <regex>
#include <fstream>
#include <iostream>
#include <ctime>
#include <unordered_map>
#ifdef _MSC_VER
#include <cstdio>
namespace std {
    using ::_snprintf; // global _snprintf’ı std:: içine al
}
#endif

#include "json.hpp"

#include <extdll.h>	
#include <meta_api.h>
#include <cstddef>
#include <casmodule.h>

#include "signatures.h"
#include "angelscript_addon.h"
#include "asext_api.h"
#include "constants.h"
#include "casbasegcobject.h"
#include "sven_enhancer_as.h"
#include "globaltypes.h"
#include "utilities.h"

#include "extern_hook.h"
#include "angelscript_helper.h"
#include <jvalue.h>
#include <moduledata.h>
#include <callbackmethod.h>
#include <curlfn.h>
#include <rest.h>