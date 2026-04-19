

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
    "${CMAKE_SOURCE_DIR}/src/thirdparty/mysql-connector/include"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/xxhash/"
    "${CMAKE_SOURCE_DIR}/src/rest"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/picosha2/"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/hash-library/"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/sqlite3/"
)


# Plugin kaynak
add_library(SvenEnhancer SHARED src/SvenEnhancer.cpp "src/header/stock/signatures.h" "src/header/stock/extern_hook.h" "src/angelscript/include/angelscript_addon.h" "src/angelscript/source/angelscript_addon.cpp" "src/engine/engine_hook.cpp" "src/engine/dll_hook.cpp" "src/header/stock/vftable.h" "src/header/stock/utility.h" "src/stock/dll_api.cpp" "src/stock/engine_api.cpp" "src/stock/vftable.cpp" "src/stock/sdk_util.cpp" "src/stock/h_export.cpp" "src/header/angelscript_expansion.h" "src/angelscript_expansion.cpp" "src/header/constants.h" "src/stock/constants.cpp" "src/utilities.cpp" "src/header/utilities.h" "src/header/stock/svenenhancer.h" "src/header/stock/angelscript_helper.h" "src/stock/algescript_helper.cpp" "src/header/stock/globaltypes.h" "src/stock/globaltypes.cpp" "src/header/stock/sven_enhancer_as.h" "src/stock/sven_enhancer_as.cpp" "src/header/stock/casbasegcobject.h" "src/stock/casbasegcobject.cpp" "src/header/stock/jvalue.h" "src/stock/jvalue.cpp" "src/header/mysql_sven.h" "src/mysql_sven.cpp" "src/header/mysql_connection_pool.h" "src/mysql_connection_pool.cpp" "src/header/callbackitem.h" "src/callbackitem.cpp" "src/header/stock/moduledata.h" "src/moduledata.cpp" "src/header/stock/callbackmethod.h" "src/callbackmethod.cpp" "src/header/stock/casmodule.h" "src/stock/casmodule.cpp" "src/rest/restclient.cpp"  "src/rest/rest.h" "src/rest/restrequest.cpp" "src/rest/restresponse.cpp" "src/rest/curlfn.h" "src/rest/curlfn.cpp" "src/rest/restcontext.cpp" "src/rest/stringcollection.cpp" "src/rest/stringcollection.h" "src/rest/restutils.cpp" "src/rest/base64.cpp" "src/rest/base64.h" "src/rest/crypto.h" "src/rest/crypto.cpp"    "src/header/sqlite3_sven.h"  "src/sqlite3_sven.cpp" )
file(GLOB HASH_SOURCES
    "${CMAKE_SOURCE_DIR}/src/thirdparty/hash-library/*.cpp"
)

target_sources(SvenEnhancer PRIVATE ${HASH_SOURCES})
set(XXHASH_DIR ${CMAKE_SOURCE_DIR}/src/thirdparty/xxhash)
enable_language(C)
target_sources(SvenEnhancer PRIVATE
    ${XXHASH_DIR}/xxhash.c
)

target_include_directories(SvenEnhancer PRIVATE
    ${XXHASH_DIR}
)
set_source_files_properties(
    ${XXHASH_DIR}/xxhash.c
    PROPERTIES LANGUAGE C
)
    include_directories( 
        "${CMAKE_SOURCE_DIR}/src/thirdparty/curl/include"
        "${CMAKE_SOURCE_DIR}/src/thirdparty/curl/include/curl"
    )
if(WIN32)

    set_target_properties(SvenEnhancer PROPERTIES
        OUTPUT_NAME "SvenEnhancer"
        PREFIX ""
        SUFFIX ".dll"
    )
else()
    #find_package(CURL REQUIRED)
    #set(CURL_LIB "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/curl/libcurl.a")
    #target_link_libraries(SvenEnhancer PRIVATE "${CURL_LIB}")
    #target_link_libraries(SvenEnhancer PRIVATE z ssl crypto dl pthread)
    #target_link_libraries(SvenEnhancer PRIVATE curl)
    target_compile_options(SvenEnhancer PRIVATE -m32)
    target_link_options(SvenEnhancer PRIVATE -m32)
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

