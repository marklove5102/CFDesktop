#include "impl/early_config_stage.h"
#include "impl/early_welcome_impl.h"
#include "impl/logger_stage.h"
#include "init.h"
#include "init_chain/init_chain.h"
#include <memory>
namespace cf::desktop::init_session {
void RunEarlyInit() {
    // Registered the Sinks of inits
    early_stage::EarlyInitRunner early_runner;

    /* Stage 0: Early Config Boots */
    early_runner.register_stage_execute_before(
        []() { return std::make_unique<early_stage::EarlyConfigStage>(); });

    /* Stage 1: Logger Boots */
    early_runner.register_stage_execute_before(
        []() { return std::make_unique<early_stage::LoggerStage>(); });

    /* Finalize: Welcome Boots */
    early_runner.register_stage_back(
        []() { return std::make_unique<early_stage::EarlyWelcomeImpl>(); });

    /* Will Force Run Each when deconstruct! */
}

void ReleaseEarlyInit() {
    // Release all sources created in early inits,
    // and passing everything to the formal boot stage
}

} // namespace cf::desktop::init_session
