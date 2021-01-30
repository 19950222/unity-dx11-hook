#pragma once

#include "Il2CppClass.hpp"
#include "MonitorData.hpp"

namespace IL2CPP
{
    typedef Il2CppClass Il2CppVTable;
    typedef struct Il2CppObject
    {
        union
        {
            Il2CppClass* klass;
            Il2CppVTable* vtable;
        };
        MonitorData* monitor;
    } Il2CppObject;
}
