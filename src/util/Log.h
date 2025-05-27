#pragma once

#define FMT_UNICODE 0
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace FFV
{
class Log
{
public:
    /*
      Initializes the logger.
      Must be called before any logging function is used.
     */
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};
} // namespace FFV

#define FFV_TRACE(...) ::FFV::Log::GetLogger()->trace(__VA_ARGS__)
#define FFV_LOG(...) ::FFV::Log::GetLogger()->info(__VA_ARGS__)
#define FFV_WARN(...) ::FFV::Log::GetLogger()->warn(__VA_ARGS__)
#define FFV_ERROR(...) ::FFV::Log::GetLogger()->error(__VA_ARGS__)
#define FFV_CRITICAL(...) ::FFV::Log::GetLogger()->critical(__VA_ARGS__)
