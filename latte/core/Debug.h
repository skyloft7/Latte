#pragma once

#ifdef _WIN32
    #define latteNewBreakpoint() __debugbreak()
#else
    #define latteNewBreakpoint()
#endif