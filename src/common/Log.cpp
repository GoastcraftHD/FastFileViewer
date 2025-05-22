#include "common/Log.h"

#include "FastFileViewerPCH.h"

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

namespace FFV
{
std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init()
{
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("FFV.log", true));

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    s_Logger = std::make_shared<spdlog::logger>("FFV", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_Logger);
    s_Logger->set_level(spdlog::level::trace);
    s_Logger->flush_on(spdlog::level::trace);
}
} // namespace FFV
