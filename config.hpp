#pragma once

/**************************************************************************************************/

// Platform detection macros
#define HYDE_PLATFORM_APPLE 0
#define HYDE_PLATFORM_MICROSOFT 0

#if defined(__APPLE__)
    #undef HYDE_PLATFORM_APPLE
    #define HYDE_PLATFORM_APPLE 1
#elif defined(_MSC_VER)
    #undef HYDE_PLATFORM_MICROSOFT
    #define HYDE_PLATFORM_MICROSOFT 1
#endif

// Generic macro to check platform
#define HYDE_PLATFORM_IS(X) (HYDE_PLATFORM_##X)
/**************************************************************************************************/
