#include "common/Log.h"

#include <filesystem>

#if defined(FFV_WINDOWS)
    #define FFV_DEBUG_BREAK() __debugbreak()
#elif defined(FFV_LINUX)
    #include <signal.h>
    #define FFV_DEBUG_BREAK() raise(SIGTRAP)
#endif

#define FFV_EXPAND_MACRO(x) x

#if defined(FFV_DEBUG)
    #define FFV_ASSERT(check, msg, execute)                                                    \
        {                                                                                      \
            if (!(check))                                                                      \
            {                                                                                  \
                FFV_ERROR("Assertion failed at {0}:{1}\n{2}",                                  \
                          std::filesystem::path(__FILE__).filename().string(), __LINE__, msg); \
                FFV_DEBUG_BREAK();                                                             \
            }                                                                                  \
        }
#else
    #define FFV_ASSERT(check, msg, execute)                                                    \
        {                                                                                      \
            if (!(check))                                                                      \
            {                                                                                  \
                FFV_ERROR("Assertion failed at {0}:{1}\n{2}",                                  \
                          std::filesystem::path(__FILE__).filename().string(), __LINE__, msg); \
                FFV_EXPAND_MACRO(execute);                                                     \
            }                                                                                  \
        }

#endif
