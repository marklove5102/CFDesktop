#include "system/network/network.h"

#include <QCoreApplication> // 仅为跑 demo 需要；实际调用方可以是纯 std 程序
#include <cstdio>

// ── 小工具：打印单张网卡 ──────────────────────────────────────
static void printInterface(const cf::InterfaceInfo& iface) {
    std::printf("  %-18s  MAC: %-19s  MTU: %d  Type: %s\n", iface.name.c_str(),
                iface.mac.empty() ? "(none)" : iface.mac.c_str(), iface.mtu,
                cf::interfaceTypeName(iface.type));

    std::printf("       Flags: Up=%d Running=%d Loopback=%d P2P=%d Broadcast=%d Multicast=%d\n",
                iface.flags.isUp(), iface.flags.isRunning(), iface.flags.isLoopback(),
                iface.flags.isPointToPoint(), iface.flags.canBroadcast(),
                iface.flags.canMulticast());

    for (const auto& e : iface.addresses) {
        std::printf("       %-39s  /%d  %s  permanent=%d\n", e.ip.toString().c_str(),
                    e.prefixLength, e.ip.isIPv4() ? "IPv4" : "IPv6", e.isPermanent);
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // ═══════════════════════════════════════
    //  一行调用
    // ═══════════════════════════════════════
    auto __info = cf::getNetworkInfo();
    if (!__info) {
        return -1;
    }
    auto info = std::move(__info.value());
    if (info.extensions) {
        std::printf("Owns Interfaces Extension, you can try to check it later!");
    }

    std::printf("=== All Interfaces (%zu) ===\n", info.interfaces.size());
    for (const auto& iface : info.interfaces)
        printInterface(iface);

    std::printf("\n=== Active Physical Interfaces ===\n");
    for (const auto& iface : info.interfaces) {
        if (iface.flags.isLoopback())
            continue;
        if (!iface.flags.isUp() || !iface.flags.isRunning())
            continue;
        printInterface(iface);

        if (auto v4 = iface.firstIPv4())
            std::printf("       → first IPv4: %s\n", v4->toString().c_str());
        if (auto v6 = iface.firstIPv6())
            std::printf("       → first IPv6: %s\n", v6->toString().c_str());
    }

    // ── 3. 全局连通状态 ──────────────────
    std::printf("\n=== Network Status ===\n");
    const auto& s = info.status;
    if (!s.backendAvailable) {
        std::printf("  QNetworkInformation backend not available.\n");
    } else {
        const char* reach[] = {"Unknown", "Disconnected", "Local", "Site", "Online"};
        const char* medium[] = {"Unknown", "Ethernet", "Cellular", "WiFi", "Bluetooth"};
        std::printf("  Reachability : %s\n", reach[static_cast<int>(s.reachability)]);
        std::printf("  Transport    : %s\n", medium[static_cast<int>(s.transportMedium)]);
        std::printf("  Metered      : %d\n", s.isMetered);
        std::printf("  CaptivePortal: %d\n", s.isBehindCaptivePortal);
    }

    return 0;
}