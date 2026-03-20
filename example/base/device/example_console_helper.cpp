#include "device/console/console_helper.h"
#include <any>
#include <cstring>
#include <iostream>
#include <string>

namespace {

// ANSI color codes - use unique names to avoid macro conflicts
namespace Colors {
constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BLUE = "\033[34m";
constexpr const char* MAGENTA = "\033[35m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* BOLD = "\033[1m";
constexpr const char* DIM = "\033[2m";
} // namespace Colors

class ColorPrinter {
  public:
    ColorPrinter() : colorful_(false) {}

    void set_colorful(bool enable) { colorful_ = enable; }
    bool is_colorful() const { return colorful_; }

    void print(const char* color, const char* text) const {
        if (colorful_) {
            std::cout << color << text << Colors::RESET;
        } else {
            std::cout << text;
        }
    }

    void print_header(const char* title) const {
        const int width = 50;
        const int title_len = static_cast<int>(std::strlen(title));
        const int padding = (width - title_len - 2) / 2;

        std::cout << '\n';
        print(Colors::CYAN, "┌");
        for (int i = 0; i < width - 2; ++i)
            std::cout << "─";
        print(Colors::CYAN, "┐\n");

        print(Colors::CYAN, "│");
        for (int i = 0; i < padding; ++i)
            std::cout << ' ';
        print(Colors::BOLD, title);
        for (int i = 0; i < width - padding - title_len - 2; ++i)
            std::cout << ' ';
        print(Colors::CYAN, "│\n");

        print(Colors::CYAN, "└");
        for (int i = 0; i < width - 2; ++i)
            std::cout << "─";
        print(Colors::CYAN, "┘\n\n");
    }

    void print_separator() const {
        print(Colors::DIM, "──────────────────────────────────────────────────");
        std::cout << '\n';
    }

    void print_info(const char* label, const char* value) const {
        print(Colors::BLUE, "  [+] ");
        print(Colors::BOLD, label);
        std::cout << ": ";
        print(Colors::GREEN, value);
        std::cout << '\n';
    }

    void print_size_info(int rows, int cols) const {
        std::cout << "     ";
        print(Colors::CYAN, "┌─");
        for (int i = 0; i < cols && i < 35; ++i)
            std::cout << "─";
        print(Colors::CYAN, "─┐\n");

        for (int i = 0; i < rows && i < 6; ++i) {
            std::cout << "     ";
            print(Colors::CYAN, "│ ");
            if (i == 0) {
                print(Colors::DIM, " console preview ");
                for (int j = 18; j < cols && j < 35; ++j)
                    std::cout << " ";
            } else if (i == rows - 1 || i == 5) {
                char buf[32];
                std::snprintf(buf, sizeof(buf), " rows: %d ", rows);
                print(Colors::DIM, buf);
                for (int j = static_cast<int>(std::strlen(buf)); j < cols && j < 35; ++j)
                    std::cout << " ";
            } else {
                for (int j = 0; j < cols && j < 35; ++j)
                    std::cout << "·";
            }
            print(Colors::CYAN, " │\n");
        }

        std::cout << "     ";
        print(Colors::CYAN, "└─");
        for (int i = 0; i < cols && i < 35; ++i)
            std::cout << "─";
        print(Colors::CYAN, "─┘\n");
        std::cout << "       ";
        print(Colors::DIM, "columns: ");
        print(Colors::YELLOW, std::to_string(cols).c_str());
        std::cout << "\n\n";
    }

  private:
    bool colorful_;
};

} // anonymous namespace

int main() {
    using namespace cf::base::device::console;

    ConsoleHelper console;
    ColorPrinter printer;

    // Check if console supports color
    std::any color_value;
    bool has_color_prop = console.query_property("colorable", &color_value);
    if (has_color_prop) {
        printer.set_colorful(std::any_cast<bool>(color_value));
    }

    printer.print_header("Console Information Demo");

    // Query and display console size
    auto size = console.size();
    if (size.has_value()) {
        const auto& console_size = size.value();
        const int rows = console_size.first;
        const int cols = console_size.second;

        printer.print_info("Console Size", "Detected");
        printer.print_size_info(rows, cols);
    } else {
        printer.print_info("Console Size", "Not Available");
        std::cout << "\n  Could not determine console size.\n";
        std::cout << "  This may happen if:\n";
        std::cout << "    * Running in a non-interactive shell\n";
        std::cout << "    * Output is redirected to a file or pipe\n";
        std::cout << "    * Terminal doesn't support size queries\n\n";
    }

    printer.print_separator();

    // Display color support info
    if (printer.is_colorful()) {
        printer.print_info("Color Support", "Enabled");
        std::cout << "\n  ";
        printer.print(Colors::RED, "● Red ");
        printer.print(Colors::GREEN, "● Green ");
        printer.print(Colors::YELLOW, "● Yellow ");
        printer.print(Colors::BLUE, "● Blue ");
        printer.print(Colors::MAGENTA, "● Magenta ");
        printer.print(Colors::CYAN, "● Cyan ");
        printer.print(Colors::BOLD, "● Bold ");
        printer.print(Colors::DIM, "● Dim");
        std::cout << Colors::RESET << "\n\n";
    } else {
        printer.print_info("Color Support", "Disabled");
        std::cout << "\n  Color output is disabled because:\n";
        std::cout << "    * NO_COLOR environment variable is set\n";
        std::cout << "    * Output is not a TTY (terminal)\n";
        std::cout << "    * TERM variable is set to 'dumb'\n\n";
    }

    // Display policy chain info
    printer.print_separator();
    std::cout << "\n  ";
    printer.print(Colors::BOLD, "Policy Chain Info:");
    std::cout << "\n\n";
    std::cout << "  Console size detection uses a fallback policy chain:\n";
    std::cout << "    ";
    printer.print(Colors::GREEN, "[1] Primary:");
    std::cout << " ioctl(TIOCGWINSZ) system call\n";
    std::cout << "    ";
    printer.print(Colors::YELLOW, "[2] Fallback:");
    std::cout << " COLUMNS/LINES environment variables\n\n";

    printer.print_separator();

    std::cout << "\n  ";
    printer.print(Colors::CYAN, "Platform: ");
#ifdef CFDESKTOP_OS_WINDOWS
    printer.print(Colors::BOLD, "Windows");
#elif defined(CFDESKTOP_OS_LINUX)
    printer.print(Colors::BOLD, "Linux");
#elif defined(CFDESKTOP_OS_MACOS)
    printer.print(Colors::BOLD, "macOS");
#else
    printer.print(Colors::BOLD, "Unknown");
#endif
    std::cout << "\n\n";

    return 0;
}
