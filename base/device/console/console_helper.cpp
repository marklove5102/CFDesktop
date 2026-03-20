#include "console_helper.h"
#include "impl/console_platform.h"

namespace cf::base::device::console {

ConsoleHelper::ConsoleHelper() : size_querier(get_platform_size_policy()) {}

std::optional<console_size_t> ConsoleHelper::size() const {
    if (!size_querier) {
        return {};
    }
    return size_querier->execute();
}

bool ConsoleHelper::query_property(const std::string_view property, std::any* value) {
    if (property == "colorable") {
        *value = platform_console_support();
        return true;
    }
    return false;
}

console_size_t ConsoleHelper::fallback_size() const {
    return {80, 24};
}

} // namespace cf::base::device::console
