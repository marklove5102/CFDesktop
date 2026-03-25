#include "early_gain/early_pass_stage.h"
#include "gui_progress/gui_init_stage.h"
#include "init_session_chain.h"
#include <memory>

namespace cf::desktop::init_session {
void RunStageInit() {
    init_session::InitSessionChain initSessionChain;

    initSessionChain.add_stage_back(std::make_unique<GuiLogoBootStage>());
    initSessionChain.add_stage_back(std::make_unique<EarlyTransferStage>());
}
} // namespace cf::desktop::init_session