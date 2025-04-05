#include "Core/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init(){
    spdlog::set_pattern("%n: %v %$");
    s_Logger=spdlog::stdout_color_mt("Logger");
    s_Logger->set_level(spdlog::level::trace);
}