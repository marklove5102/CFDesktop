# OS 助手

`os_helper.h` 提供操作系统层面的通用辅助函数，用于查询系统信息和执行平台相关操作。这个模块的设计思路和 CPU、Memory 模块类似——为跨平台差异提供统一的接口，让上层业务代码不需要关心底层系统的具体实现。

## 头文件

```cpp
#include "base/system/os_helper.h"
```

## 设计动机

操作系统信息查询是桌面应用开发中的常见需求——获取系统版本、检测环境特征、判断运行平台等。这些操作在 Windows 和 Linux 上完全不同，但业务层只需要一个统一的答案。OS 助手的作用就是把这些差异封装起来，提供一个干净的 API。

## 使用场景

### 平台判断

虽然编译时的平台检测可以用 `system_judge.h` 里的宏，但运行时判断有时是必需的——比如同一个二进制需要在多个平台版本上运行，或者需要根据系统版本动态调整行为：

```cpp
#include "base/system/os_helper.h"

void configure_application() {
    // 根据系统版本选择合适的默认配置
    if (cf::isWindows11OrLater()) {
        enable_rounded_corners(true);
        enable_mica_material(true);
    } else if (cf::isWindows10OrLater()) {
        enable_acrylic_blur(true);
    }
}
```

### 系统特性检测

某些系统特性可能只在特定版本上可用，运行时检测可以避免在不支持的环境上调用不存在的 API：

```cpp
void setup_theme() {
    if (cf::supports_dark_mode()) {
        enable_dark_mode(true);
    }

    if (cf::supports_virtual_desktop()) {
        setup_virtual_desktop_integration();
    }
}
```

## API 结构

OS 助手模块的函数按功能分组：

### 系统信息

| 函数 | 说明 |
|------|------|
| `getOSName()` | 获取操作系统名称 |
| `getOSVersion()` | 获取系统版本号 |
| `getArchitecture()` | 获取 CPU 架构 |

### 特性检测

| 函数 | 说明 |
|------|------|
| `supportsDarkMode()` | 是否支持深色模式 |
| `isWindows10OrLater()` | 是否为 Windows 10 或更高版本 |
| `isWindows11OrLater()` | 是否为 Windows 11 或更高版本 |

### 路径和环境

| 函数 | 说明 |
|------|------|
| `getConfigPath()` | 获取配置文件目录 |
| `getDataPath()` | 获取应用数据目录 |
| `getCachePath()` | 获取缓存目录 |

## 平台差异

| 功能 | Windows | Linux |
|------|---------|-------|
| 系统版本 | `GetVersionEx` / RtlGetVersion | `/etc/os-release` 解析 |
| 路径获取 | `SHGetFolderPath` / `KnownFolders` | `XDG_*` 环境变量 |
| 特性检测 | API 可用性检查 | 特性宏或运行时测试 |

## 线程安全

OS 助手的所有函数都是线程安全的。内部使用缓存的系统信息在首次查询后会被保存，后续调用直接返回缓存值。如果需要刷新信息（比如系统版本可能在运行时升级），可以调用对应的 `*_force_refresh()` 函数。

## 注意事项

⚠️ Windows 上获取系统版本需要注意版本欺骗问题。从 Windows 10 开始，应用程序需要在 manifest 中声明 `supportedOS` 才能获取正确的版本号，否则可能返回 Windows 8 的版本号。我们的实现会尝试使用 `RtlGetVersion` 来绕过这个限制。

⚠️ Linux 上的"版本"概念比 Windows 复杂。发行版版本（如 Ubuntu 22.04）、内核版本（如 5.15）、桌面环境版本（如 GNOME 42）是三个不同的东西。`getOSVersion()` 返回的是发行版版本，如果你需要内核版本，应该使用 `getKernelVersion()`。

⚠️ 路径相关函数返回的路径可能不存在，调用者需要负责创建目录。这是有意的设计——我们只负责告诉标准位置在哪里，不负责创建目录结构。

## 完整示例

```cpp
#include "base/system/os_helper.h"
#include <iostream>

void print_system_info() {
    std::cout << "=== 系统信息 ===" << std::endl;
    std::cout << "操作系统: " << cf::getOSName() << std::endl;
    std::cout << "版本: " << cf::getOSVersion() << std::endl;
    std::cout << "架构: " << cf::getArchitecture() << std::endl;

    std::cout << "\n=== 路径 ===" << std::endl;
    std::cout << "配置: " << cf::getConfigPath() << std::endl;
    std::cout << "数据: " << cf::getDataPath() << std::endl;
    std::cout << "缓存: " << cf::getCachePath() << std::endl;

    std::cout << "\n=== 特性支持 ===" << std::endl;
    std::cout << "深色模式: " << (cf::supports_dark_mode() ? "是" : "否") << std::endl;
}

int main() {
    print_system_info();
    return 0;
}
```

## 相关文档

- [CPU 基础信息 API](./cpu/cfcpu.md)
- [Memory 信息 API](./memory/memory_info.md)
- [system_judge - 系统判断宏](../../base/macro/system_judge.md)
