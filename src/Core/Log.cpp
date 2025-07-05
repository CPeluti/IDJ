#include "Core/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/msvc_sink.h>
#include <memory>
std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init(){

    // Create a shared pointer to the msvc sink
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

    //// Create a logger that uses this sink
    s_Logger = std::make_shared<spdlog::logger>("msvc_logger", sink);

    //// Set as default logger to use spdlog::info(...) globally
    spdlog::set_default_logger(s_Logger);

    //// Set log level and flush policy
    s_Logger->set_level(spdlog::level::debug);
    s_Logger->flush_on(spdlog::level::debug);

    spdlog::set_pattern("%n: %v %$");
    s_Logger->set_level(spdlog::level::trace);
}