cmake_minimum_required(VERSION 3.25)
project(SvenEnhancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SDK include dizinleri
include_directories(
    "${CMAKE_SOURCE_DIR}/../metamod-fg/metamod"  # metamod
    "${CMAKE_SOURCE_DIR}/../metamod-fg/hlsdk/common"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/hlsdk/dlls"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/hlsdk/engine"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/hlsdk/pm_shared"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/asext/include"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/thirdparty/angelscript-sdk/angelscript/include"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/thirdparty/angelscript-sdk/add_on/scriptdictionary"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/thirdparty/angelscript-sdk/add_on/scriptarray"
    "${CMAKE_SOURCE_DIR}/../metamod-fg/thirdparty/angelscript-sdk/add_on/scriptany"
    "${CMAKE_SOURCE_DIR}/src/header"
    "${CMAKE_SOURCE_DIR}/src/header/stock"
    "${CMAKE_SOURCE_DIR}/src/angelscript"
    "${CMAKE_SOURCE_DIR}/src/angelscript/header"
    "${CMAKE_SOURCE_DIR}/src/angelscript/include"
)

# Plugin kaynak
add_library(SvenEnhancer SHARED src/SvenEnhancer.cpp "src/header/stock/signatures.h" "src/header/stock/extern_hook.h" "src/angelscript/include/angelscript_addon.h" "src/angelscript/source/angelscript_addon.cpp" "src/engine/engine_hook.cpp" "src/engine/dll_hook.cpp" "src/header/stock/vftable.h" "src/header/stock/utility.h" "src/stock/dll_api.cpp" "src/stock/engine_api.cpp" "src/stock/vftable.cpp" "src/stock/sdk_util.cpp" "src/stock/h_export.cpp" "src/header/angelscript_expansion.h" "src/angelscript_expansion.cpp" "src/header/constants.h" "src/stock/constants.cpp" "src/utilities.cpp" "src/header/utilities.h" "src/header/stock/svenenhancer.h" "src/header/stock/angelscript_helper.h" "src/stock/algescript_helper.cpp" "src/header/stock/globaltypes.h" "src/stock/globaltypes.cpp" "src/header/stock/sven_enhancer_as.h" "src/stock/sven_enhancer_as.cpp" "src/header/stock/casbasegcobject.h" "src/stock/casbasegcobject.cpp" "src/header/stock/jvalue.h" "src/stock/jvalue.cpp")



if(WIN32)
    set_target_properties(SvenEnhancer PROPERTIES
        OUTPUT_NAME "SvenEnhancer"
        PREFIX ""
        SUFFIX ".dll"
    )
else()
    set_target_properties(SvenEnhancer PROPERTIES
        OUTPUT_NAME "SvenEnhancer"
        PREFIX ""
        SUFFIX ".so"
    )

    set_target_properties(SvenEnhancer PROPERTIES
        COMPILE_FLAGS "-m32"
        LINK_FLAGS "-m32"
    )
endif()

