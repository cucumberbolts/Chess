#pragma once

#if defined (OS_LINUX)
#include <signal.h>
void __debugbreak()
{
    raise (SIGTRAP);
}
#endif

namespace DebugContext {
    void Init();  // Only initializes if GLFW_CONTEXT_DEBUG is true
}
