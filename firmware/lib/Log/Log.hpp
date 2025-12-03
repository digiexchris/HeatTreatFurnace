#pragma once

#include <filesystem>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/syslog_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/fmt/bin_to_hex.h"

class Log
{
public:
    struct Config
    {
        struct Console
        {
            bool enable = true;
        } console;

        struct File
        {
            bool enable = false;
            std::filesystem::path path;
            std::size_t maxFileSize = 10000;
            std::size_t maxFiles = 10;
        } file;

        struct Callback
        {
            bool enable = false;
            spdlog::custom_log_callback callback;
        } callback;
    } myConfig;

    explicit Log(Config aConfig);

    ~Log() = default;

    void operator()(std::string aMsg) const
    {
        Info(aMsg);
    }

    void Error(const std::string &aMsg) const;
    void Warn(const std::string &aMsg) const;
    void Info(const std::string& aMsg) const;
    void Debug(const std::string& aMsg) const;

protected:
    // std::unique_ptr<spdlog::sinks::stdout_color_sink_mt> myConsoleSink;
    // std::unique_ptr<spdlog::sinks::rotating_file_sink_mt> myFileSink;
    // std::unique_ptr<spdlog::sinks::syslog_sink_mt> mySyslogSink;
    // std::unique_ptr<spdlog::sinks::callback_sink_mt> myCallbackSink;
    std::vector<spdlog::sink_ptr> mySinks;
    std::unique_ptr<spdlog::logger> myLogger;
};
