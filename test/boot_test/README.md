# Boot Test - Qt 编译环境检测工具

## 项目简介

这是一个迷你的 Qt 检测程序，用于在 boot 编译时检测 Qt 环境是否正确配置。

**工具链要求：**
- LLVM/Clang 编译器
- Qt 6.8.3 或更高版本

## 项目结构

```
boot_test/
├── CMakeLists.txt          # CMake 构建配置
├── boot_detect.h           # 检测逻辑头文件
├── boot_detect.cpp         # 检测逻辑实现
├── boot_test_core.cpp      # 控制台版本主程序 (QCoreApplication)
├── boot_test_gui.cpp       # GUI 版本主程序 (QApplication)
└── README.md               # 本文件
```

## 编译方法

### 前置要求

- CMake 3.16+
- Qt 6.8.3+
- LLVM/Clang (或 MSVC with Qt)

### 构建步骤

```bash
# 进入项目目录
cd test/boot_test

# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake .. -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

# 或者在 Windows with Visual Studio 上
cmake .. -G "Visual Studio 17 2022"

# 编译
cmake --build . --config Release

# 或使用 Ninja
ninja
```

### Qt CMake 配置

如果 CMake 找不到 Qt，可以设置 Qt 路径：

```bash
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
```

## 使用方法

### 控制台版本 (boot_test_core)

```bash
# 基本使用（简洁输出）
./boot_test_core

# 详细输出
./boot_test_core -v

# 或
./boot_test_core --verbose

# 安静模式（只输出错误）
./boot_test_core -q
```

**退出码：**
- `0` - 所有检测通过
- `1` - 有检测失败

### GUI 版本 (boot_test_gui)

```bash
# 直接运行
./boot_test_gui
```

GUI 窗口会显示所有检测结果，并提供：
- 重新运行测试按钮
- 退出按钮

## 检测项目

1. **Qt 版本检测** - 检查 Qt 版本是否 >= 6.8.0
2. **Qt 模块检测** - 检查 Core、Gui、Widgets 模块是否可用
3. **编译器检测** - 检测是否使用 LLVM/Clang（警告但不失败）
4. **系统信息** - 显示操作系统、架构等信息

## 集成到构建脚本

### CMake 示例

```cmake
# 在主项目的 CMakeLists.txt 中
find_package(Qt6 REQUIRED COMPONENTS Core)

# 添加子目录
add_subdirectory(test/boot_test)

# 在构建前运行检测
add_custom_target(boot_check
    COMMAND boot_test_core
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Checking boot environment..."
)

add_dependencies(your_main_target boot_check)
```

### 批处理脚本示例 (Windows)

```batch
@echo off
echo Running boot environment check...

boot_test_core.exe
if errorlevel 1 (
    echo Boot check FAILED!
    exit /b 1
)

echo Boot check PASSED!
echo Continuing build...
```

### Shell 脚本示例 (Linux/macOS)

```bash
#!/bin/bash

echo "Running boot environment check..."

if ./boot_test_core; then
    echo "Boot check PASSED!"
    echo "Continuing build..."
else
    echo "Boot check FAILED!"
    exit 1
fi
```

## 常见问题

### Q: CMake 找不到 Qt6？
**A:** 设置 `CMAKE_PREFIX_PATH` 指向 Qt 安装目录：
```bash
cmake .. -DCMAKE_PREFIX_PATH="/path/to/Qt/6.8.3/compiler"
```

### Q: 编译时找不到 Qt 头文件？
**A:** 确保 Qt 路径正确，并且使用正确的编译器配置：
```bash
qmake -query
```

### Q: Windows 下控制台窗口一闪而过？
**A:** 使用 `boot_test_gui.exe` 或在命令行中运行 `boot_test_core.exe`

## 许可证

本工具随主项目使用相同的许可证。
