#include "cflog.h"
#include "cflog/cflog.hpp"
#include "cflog/cflog_format_factory.h"
#include "cflog/formatter/console_formatter.h"
#include "cflog/sinks/console_sink.h"
#include <memory>

static cf::log::FormatterFactory factory;

void run_logger_init() {
    using namespace cf::log;

    factory.register_formatter("console", []() { return std::make_shared<AsciiColorFormatter>(); });

    auto console_sink = std::make_shared<ConsoleSink>();
    console_sink->setFormat(factory.create("console"));
    Logger::instance().add_sink(console_sink);

    // Set log level using the function from cflog.h
    cf::log::set_level(cf::log::level::INFO);
}
