# Memory 信息 API

`memory_info.h` 提供了系统内存信息的查询接口——包括物理内存使用情况、交换空间、缓存内存、当前进程内存占用，以及物理内存条（DIMM）的详细信息。这是和 CPU 模块同一系列的设计，同样采用跨平台抽象，但 Memory 模块使用输出参数而不是 `expected` 返回值，主要因为内存查询失败的情况相对少见，而且我们确实需要填充一个较大的结构体。

## 头文件

```cpp
#include "system/memory/memory_info.h"
```

## 基本用法

调用 `getSystemMemoryInfo()` 并传入一个 `MemoryInfo` 引用，函数会填充所有可用的信息：

```cpp
#include "system/memory/memory_info.h"
#include <iostream>

int main() {
    cf::MemoryInfo info;
    cf::getSystemMemoryInfo(info);

    // 物理内存
    std::cout << "物理内存: "
              << info.physical.total_bytes / 1024 / 1024 / 1024 << " GB"
              << std::endl;
    std::cout << "可用: "
              << info.physical.available_bytes / 1024 / 1024 / 1024 << " GB"
              << std::endl;

    return 0;
}
```

## 数据结构

`MemoryInfo` 是一个聚合结构，包含所有内存相关的信息：

```cpp
struct MemoryInfo {
    PhysicalMemory physical;     // 物理内存统计
    SwapMemory swap;             // 交换空间统计
    CachedMemory cached;         // 缓存/缓冲区内存（主要是 Linux）
    ProcessMemory process;       // 当前进程内存使用
    std::vector<DimmInfo> dimms; // 内存条（DIMM）信息列表
};
```

### PhysicalMemory

描述物理内存的使用情况：

| 字段 | 类型 | 说明 |
|------|------|------|
| `total_bytes` | `uint64_t` | 总物理内存大小 |
| `available_bytes` | `uint64_t` | 可用内存大小（包含可回收的缓存） |
| `free_bytes` | `uint64_t` | 完全空闲的内存大小 |

⚠️ `available_bytes` 和 `free_bytes` 的含义不同。Linux 上 `available` 是系统认为"可用于分配新内存"的总量，包含了可以回收的缓存；Windows 上这个概念类似但计算方式略有差异。判断系统内存压力时应该用 `available_bytes`。

### SwapMemory

交换空间（虚拟内存/页面文件）的使用情况：

| 字段 | 类型 | 说明 |
|------|------|------|
| `total_bytes` | `uint64_t` | 交换空间总大小 |
| `free_bytes` | `uint64_t` | 剩余交换空间大小 |

### CachedMemory

Linux 特有的缓存内存分类：

| 字段 | 类型 | 说明 |
|------|------|------|
| `buffers_bytes` | `uint64_t` | 块设备缓冲区占用 |
| `cached_bytes` | `uint64_t` | 页面缓存占用 |
| `shared_bytes` | `uint64_t` | 共享内存（tmpfs）占用 |
| `slab_bytes` | `uint64_t` | 内核数据结构占用 |

Windows 上这些字段会保持为零值，因为不适用这个内存模型。

### ProcessMemory

当前进程的内存使用情况：

| 字段 | 类型 | 说明 |
|------|------|------|
| `vm_rss_bytes` | `uint64_t` | 驻留集大小——实际占用的物理内存 |
| `vm_size_bytes` | `uint64_t` | 虚拟内存总大小 |
| `vm_peak_bytes` | `uint64_t` | 虚拟内存历史峰值 |

⚠️ `vm_size_bytes` 通常远大于 `vm_rss_bytes`，因为包含了所有已分配但尚未物理占用的虚拟地址空间。关注进程实际内存占用时应该看 `vm_rss_bytes`。

### DimmInfo

物理内存条的详细信息：

| 字段 | 类型 | 说明 |
|------|------|------|
| `capacity_bytes` | `uint64_t` | 内存条容量 |
| `type` | `MemoryType` | 内存类型（DDR3/DDR4/LPDDR4 等） |
| `frequency_mhz` | `uint32_t` | 运行频率（MHz） |
| `manufacturer` | `string` | 制造商名称（可能为空） |
| `part_number` | `string` | 型号/料号（可能为空） |
| `serial_number` | `string` | 序列号（可能为空） |
| `channel` | `uint8_t` | 内存通道编号（0-based） |
| `slot` | `uint8_t` | 插槽编号（0-based） |

`MemoryType` 枚举支持 DDR2/3/4/5、LPDDR3/4/4X/5 以及传统 SDRAM。如果检测失败会返回 `UNKNOWN`。

## 内存使用率计算

计算内存使用率需要注意使用正确的字段：

```cpp
cf::MemoryInfo info;
cf::getSystemMemoryInfo(info);

// 计算使用率（使用 available 而不是 free）
double usage_ratio = 1.0 - static_cast<double>(info.physical.available_bytes)
                           / static_cast<double>(info.physical.total_bytes);

std::cout << "内存使用率: " << usage_ratio * 100 << "%" << std::endl;
```

只使用 `free_bytes` 计算使用率在 Linux 上会高估，因为系统会主动使用空闲内存做缓存。

## 内存条信息

遍历内存条列表可以获取每根内存的详细信息：

```cpp
for (const auto& dimm : info.dimms) {
    std::cout << "通道 " << (int)dimm.channel
              << " 插槽 " << (int)dimm.slot << ": "
              << dimm.capacity_bytes / 1024 / 1024 << " MB";

    // 打印内存类型
    const char* type_names[] = {
        "Unknown", "DDR2", "DDR3", "DDR4", "DDR5",
        "LPDDR3", "LPDDR4", "LPDDR4X", "LPDDR5", "SDRAM"
    };
    std::cout << " " << type_names[static_cast<int>(dimm.type)];

    // 打印频率
    if (dimm.frequency_mhz > 0) {
        std::cout << " @" << dimm.frequency_mhz << " MHz";
    }

    // 打印制造商（如果有）
    if (!dimm.manufacturer.empty()) {
        std::cout << " (" << dimm.manufacturer << ")";
    }

    std::cout << std::endl;
}
```

## 平台差异

| 功能 | Windows | Linux |
|------|---------|-------|
| 物理内存 | `GlobalMemoryStatusEx` | `/proc/meminfo` 解析 |
| 交换空间 | 页面文件统计 | `/proc/meminfo` Swap* 字段 |
| 缓存内存 | 不适用 | `Buffers`/`Cached`/`SReclaimable` |
| 进程内存 | `GetProcessMemoryInfo` | `/proc/self/status` |
| DIMM 信息 | SMBIOS/DMI | `dmidecode` 或 `/proc/meminfo` |

Windows 下 DIMM 信息需要 WMI 或 SMBIOS 访问，可能需要管理员权限。Linux 下完整的 DIMM 信息需要 `dmidecode`，这通常需要 root 权限；如果权限不足，`dimms` 列表可能为空或只包含部分信息。

## 注意事项

1. **权限问题**：获取完整的 DIMM 信息通常需要提升权限。普通用户运行时可能只有基础的物理/交换内存数据。

2. **Linux 缓存行为**：Linux 会把空闲内存用作页面缓存，这导致 `free_bytes` 看起来很小。这是正常行为，不是内存泄漏。判断是否需要增加内存应该看 `available_bytes`。

3. **32 位限制**：在 32 位进程上，某些字段可能被截断。我们的实现统一使用 `uint64_t`，但底层 API 在 32 位环境下可能有精度限制。

## 完整示例

```cpp
#include "system/memory/memory_info.h"
#include <iostream>
#include <iomanip>

auto operator"" _GB(unsigned long long bytes) -> double {
    return static_cast<double>(bytes) / 1024.0 / 1024.0 / 1024.0;
}

auto operator"" _MB(unsigned long long bytes) -> double {
    return static_cast<double>(bytes) / 1024.0 / 1024.0;
}

void print_memory_info(const cf::MemoryInfo& info) {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "=== 物理内存 ===" << std::endl;
    std::cout << "  总计:   " << info.physical.total_bytes _GB << " GB" << std::endl;
    std::cout << "  可用:   " << info.physical.available_bytes _GB << " GB" << std::endl;
    std::cout << "  空闲:   " << info.physical.free_bytes _GB << " GB" << std::endl;

    double usage = (1.0 - static_cast<double>(info.physical.available_bytes) /
                          static_cast<double>(info.physical.total_bytes)) * 100.0;
    std::cout << "  使用率: " << usage << "%" << std::endl;

    std::cout << "\n=== 交换空间 ===" << std::endl;
    if (info.swap.total_bytes > 0) {
        std::cout << "  总计: " << info.swap.total_bytes _GB << " GB" << std::endl;
        std::cout << "  空闲: " << info.swap.free_bytes _GB << " GB" << std::endl;
    } else {
        std::cout << "  未配置" << std::endl;
    }

    std::cout << "\n=== 当前进程 ===" << std::endl;
    std::cout << "  RSS:   " << info.process.vm_rss_bytes _MB << " MB" << std::endl;
    std::cout << "  虚拟:  " << info.process.vm_size_bytes _MB << " MB" << std::endl;
    std::cout << "  峰值:  " << info.process.vm_peak_bytes _MB << " MB" << std::endl;

    std::cout << "\n=== 内存条 (" << info.dimms.size() << ") ===" << std::endl;
    for (const auto& dimm : info.dimms) {
        std::cout << "  通道" << (int)dimm.channel << "/插槽" << (int)dimm.slot << ": "
                  << dimm.capacity_bytes _MB << " MB";

        const char* type_str = "Unknown";
        switch (dimm.type) {
            case cf::MemoryType::DDR3: type_str = "DDR3"; break;
            case cf::MemoryType::DDR4: type_str = "DDR4"; break;
            case cf::MemoryType::DDR5: type_str = "DDR5"; break;
            case cf::MemoryType::LPDDR4: type_str = "LPDDR4"; break;
            case cf::MemoryType::LPDDR5: type_str = "LPDDR5"; break;
            default: break;
        }
        std::cout << " " << type_str;
        if (dimm.frequency_mhz > 0) {
            std::cout << " @" << dimm.frequency_mhz << " MHz";
        }
        if (!dimm.manufacturer.empty()) {
            std::cout << " (" << dimm.manufacturer << ")";
        }
        std::cout << std::endl;
    }
}

int main() {
    cf::MemoryInfo info;
    cf::getSystemMemoryInfo(info);
    print_memory_info(info);
    return 0;
}
```

## 相关文档

- [CPU 基础信息 API](../cpu/cfcpu.md)
- [系统模块概述](../cpu/overview.md)
