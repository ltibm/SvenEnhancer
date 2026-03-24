#pragma once
#include <angelscript.h>
typedef struct RefObject
{
    void* data;
    asITypeInfo* type;
} RefObject;