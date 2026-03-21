#include "system/gpu/gpu.h"
#include <iostream>

using namespace cf;

static const char* vendorName(uint32_t vendorId) {
    switch (vendorId) {
        case 0x10DE:
            return "NVIDIA";
        case 0x1002:
            return "AMD";
        case 0x8086:
            return "Intel";
        case 0xC0CA:
            return "Rockchip (Mali)";
        case 0x1957:
            return "NXP (Vivante)";
        default:
            return "Unknown";
    }
}

int main() {
    auto gpuDisplayInfo = getGpuDisplayInfo();
    if (!gpuDisplayInfo.has_value()) {
        std::cerr << "Failed to query GPU/Display info\n";
        return 1;
    }

    const auto& info = *gpuDisplayInfo;

    // ═══════════════════════════════════════════════════════════
    //  GPU Information
    // ═══════════════════════════════════════════════════════════
    std::cout << "========== GPU Information ==========\n";
    std::cout << "Name:            " << info.gpu.name << "\n";
    std::cout << "Backend:         " << info.gpu.backend << "\n";
    std::cout << "Vendor:          " << vendorName(info.gpu.vendorId) << " (0x" << std::hex
              << info.gpu.vendorId << std::dec << ")\n";
    std::cout << "Device ID:       0x" << std::hex << info.gpu.deviceId << std::dec << "\n";

    if (!info.gpu.driverVersion.empty()) {
        std::cout << "Driver Version:  " << info.gpu.driverVersion << "\n";
    }

    std::cout << "Type:            ";
    if (info.gpu.hasSoftware) {
        std::cout << "Software Rendering\n";
    } else if (info.gpu.isDiscrete) {
        std::cout << "Discrete GPU\n";
    } else if (info.gpu.isIntegrated) {
        std::cout << "Integrated GPU\n";
    } else {
        std::cout << "Unknown\n";
    }

    std::cout << "WSL Mode:        " << (info.gpu.isWSL ? "Yes" : "No") << "\n";

    // ═══════════════════════════════════════════════════════════
    //  Display Information
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n========== Display Information ==========\n";
    std::cout << "Resolution:      " << info.display.width << "x" << info.display.height << "\n";
    std::cout << "Refresh Rate:    " << info.display.refreshRate << " Hz\n";
    std::cout << "DPI:             " << info.display.dpi << "\n";
    std::cout << "Pixel Ratio:     " << info.display.devicePixelRatio << "\n";
    std::cout << "Virtual Display: " << (info.display.isVirtual ? "Yes" : "No") << "\n";

    // ═══════════════════════════════════════════════════════════
    //  Environment Performance Score
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n========== Environment Score (Max 100) ==========\n";
    std::cout << "GPU Score:       " << info.score.gpu << " / 50\n";
    std::cout << "Display Score:   " << info.score.display << " / 50\n";
    std::cout << "Total Score:     " << info.score.total << " / 100\n";
    std::cout << "Performance:     " << info.score.level << "\n";

    return 0;
}
