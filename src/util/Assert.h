#include "util/Log.h"

#include <filesystem>

#if defined(FFV_WINDOWS)
    #define FFV_DEBUG_BREAK() __debugbreak()
#elif defined(FFV_LINUX)
    #include <signal.h>
    #define FFV_DEBUG_BREAK() raise(SIGTRAP)
#endif

#if defined(FFV_DEBUG)
    /*
     * If you want to just execute and not print a message you can just use empty "" e.g.
     * FFV_ASSERT(false, "", return)
     * @param check: if false the Assert and a breakpoint gets triggerd
     * @param msg: print a debug message
     * @param execute: (optional) gets executed after the breakpoint
     */
    #define FFV_ASSERT(check, msg, execute)                                                    \
        {                                                                                      \
            if (!(check))                                                                      \
            {                                                                                  \
                FFV_ERROR("Assertion failed at {0}:{1}\n                {2}",                  \
                          std::filesystem::path(__FILE__).filename().string(), __LINE__, msg); \
                FFV_DEBUG_BREAK();                                                             \
                execute;                                                                       \
            }                                                                                  \
        }
#elif defined(FFV_DEBUG_OPT)
    /*
     * If you want to just execute and not print a message you can just use empty "" e.g.
     * FFV_ASSERT(false, "", return)
     * @param check: if false the Assert gets triggerd
     * @param msg: print a debug message
     * @param execute: (optional) gets executed after the message has been printed
     */
    #define FFV_ASSERT(check, msg, execute)                                                    \
        {                                                                                      \
            if (!(check))                                                                      \
            {                                                                                  \
                FFV_ERROR("Assertion failed at {0}:{1}\n                {2}",                  \
                          std::filesystem::path(__FILE__).filename().string(), __LINE__, msg); \
                execute;                                                                       \
            }                                                                                  \
        }
#else
    /*
     * When execute is empty nothing happens.
     * @param check: If false the Assert gets triggerd
     * @param msg: In Release mode no message is printed
     * @param execute: gets executed
     */
    #define FFV_ASSERT(check, msg, execute) \
        {                                   \
            if (!(check))                   \
            {                               \
                execute;                    \
            }                               \
        }
#endif
