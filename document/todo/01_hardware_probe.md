# Phase 1: 硬件探针与能力分级 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 2~3 周
> **依赖阶段**: Phase 0
> **目标交付物**: 硬件检测模块、能力档位枚举、配置文件系统

---

## 一、阶段目标

### 核心目标
实现开机自动硬件检测，输出能力档位枚举，作为后续所有模块的行为裁剪依据。

### 具体交付物
- [ ] `HardwareProbe` 模块及其单元测试
- [ ] `CapabilityPolicy` 策略引擎
- [ ] `HWTier` 枚举定义及查询接口
- [ ] 设备配置文件 `/etc/CFDesktop/device.conf`
- [ ] Mock 数据集用于单元测试

---

## 二、Week 1: 基础检测器实现

### Day 1-2: CPU 检测器
- [ ] 创建 `CPUDetector` 类框架
  - [ ] 定义 `CPUInfo` 结构体
  - [ ] 实现 `detectCPU()` 方法
- [ ] 实现 `/proc/cpuinfo` 解析
  - [ ] 解析 CPU 型号
  - [ ] 解析核心数
  - [ ] 解析频率
  - [ ] 解析特性 (neon, vfpv4)
- [ ] 实现设备树 compatible 读取
  - [ ] 读取 `/sys/firmware/devicetree/base/compatible`
- [ ] 实现 `uname` 架构检测
- [ ] 编写 CPU 检测单元测试
  - [ ] IMX6ULL Mock 数据
  - [ ] RK3568 Mock 数据
  - [ ] RK3588 Mock 数据

### Day 3: GPU 检测器
- [ ] 创建 `GPUDetector` 类框架
  - [ ] 定义 `GPUInfo` 结构体
- [ ] 实现 DRM 设备检测
  - [ ] 检测 `/dev/dri/card*`
- [ ] 实现 OpenGL 上下文创建
  - [ ] 获取 GPU 信息
  - [ ] 获取扩展列表
- [ ] 实现 GPU 能力查询
  - [ ] 最大纹理尺寸
  - [ ] OpenGL 版本
- [ ] 编写 GPU 检测单元测试

### Day 4: 内存与网络检测器
- [ ] 实现 `/proc/meminfo` 解析
  - [ ] 定义 `MemoryInfo` 结构体
  - [ ] 解析总内存
  - [ ] 解析可用内存
  - [ ] 解析 Swap
- [ ] 实现网络接口枚举
  - [ ] 定义 `NetworkInterface` 结构体
  - [ ] 枚举 `/sys/class/net/*`
  - [ ] 获取 IP 地址
- [ ] 编写相应单元测试

### Day 5: 档位计算逻辑
- [ ] 实现评分算法
  - [ ] CPU 评分规则
  - [ ] GPU 评分规则
  - [ ] 内存评分规则
- [ ] 定义档位阈值
  - [ ] Low: 0-60 分
  - [ ] Mid: 61-120 分
  - [ ] High: 121+ 分
- [ ] 实现 `calculateTier()` 方法
- [ ] 验证各板卡档位判定
  - [ ] IMX6ULL → Low
  - [ ] RK3568 → Mid
  - [ ] RK3588 → High

---

## 三、Week 2: 策略引擎与配置

### Day 1-2: CapabilityPolicy 实现
- [ ] 定义各策略结构体
  - [ ] `AnimationPolicy`
  - [ ] `RenderingPolicy`
  - [ ] `VideoDecoderPolicy`
  - [ ] `MemoryPolicy`
- [ ] 实现档位特定策略配置
  - [ ] Low 档策略
  - [ ] Mid 档策略
  - [ ] High 档策略
- [ ] 实现 `getAnimationPolicy()`
- [ ] 实现 `getRenderingPolicy()`
- [ ] 实现 `getVideoDecoderPolicy()`
- [ ] 实现 `getMemoryPolicy()`

### Day 3: DeviceConfig 实现
- [ ] 实现配置文件解析
  - [ ] INI 格式解析
  - [ ] 默认值处理
- [ ] 实现配置文件写入
- [ ] 实现自定义脚本执行
  - [ ] 脚本路径配置
  - [ ] JSON 输出解析
- [ ] 编写配置测试

### Day 4: 集成测试
- [ ] 创建端到端测试用例
  - [ ] 完整检测流程
  - [ ] 策略应用验证
- [ ] 测试 IMX6ULL 真机
  - [ ] CPU 信息验证
  - [ ] 档位验证
- [ ] 测试 RK3568 真机
- [ ] 测试 RK3588 真机
- [ ] 验证自动档位判定

### Day 5: 文档与优化
- [ ] 编写 API 文档
  - [ ] HardwareProbe 接口
  - [ ] CapabilityPolicy 接口
- [ ] 优化检测性能
  - [ ] 目标: < 500ms
- [ ] 添加性能日志
- [ ] Code Review

---

## 四、Week 3: 完善与验证

### Day 1-2: 跨平台支持
- [ ] 实现 Windows 平台检测
  - [ ] WMI 查询
  - [ ] GPU 检测
- [ ] 实现模拟器平台适配
  - [ ] Mock 数据支持
  - [ ] 环境变量检测
- [ ] 编写平台特定测试

### Day 3-4: 真机验证
- [ ] 在 IMX6ULL 上完整测试
  - [ ] 性能测试
  - [ ] 内存占用测试
- [ ] 在 RK3568 上完整测试
  - [ ] OpenGL 检测验证
  - [ ] 硬件解码验证
- [ ] 在 RK3588 上完整测试
  - [ ] 全功能验证
- [ ] 收集性能数据

### Day 5: 发布准备
- [ ] 最终测试
- [ ] 更新文档
- [ ] 准备演示
- [ ] 合并主分支

---

## 五、验收标准

### 功能验收
- [ ] 在 IMX6ULL 上正确识别为 Low 档
- [ ] 在 RK3568 上正确识别为 Mid 档
- [ ] 在 RK3588 上正确识别为 High 档
- [ ] 所有单元测试通过（覆盖率 > 90%）
- [ ] 配置文件覆盖功能正常
- [ ] 自定义脚本执行正常

### 性能验收
- [ ] 硬件检测耗时 < 500ms
- [ ] 内存占用 < 5MB
- [ ] 不影响系统启动时间

### 代码质量
- [ ] 符合代码规范
- [ ] 通过 clang-tidy 检查
- [ ] API 文档完整
- [ ] 代码审查通过

---

## 六、文件清单

### 头文件
- [ ] `include/CFDesktop/Base/HardwareProbe/HWTier.h`
- [ ] `include/CFDesktop/Base/HardwareProbe/HardwareInfo.h`
- [ ] `include/CFDesktop/Base/HardwareProbe/HardwareProbe.h`
- [ ] `include/CFDesktop/Base/HardwareProbe/CapabilityPolicy.h`
- [ ] `include/CFDesktop/Base/HardwareProbe/DeviceConfig.h`

### 源文件
- [ ] `src/base/hardware/HardwareProbe.cpp`
- [ ] `src/base/hardware/CapabilityPolicy.cpp`
- [ ] `src/base/hardware/DeviceConfig.cpp`
- [ ] `src/base/hardware/detectors/CPUDetector.cpp`
- [ ] `src/base/hardware/detectors/GPUDetector.cpp`
- [ ] `src/base/hardware/detectors/MemoryDetector.cpp`
- [ ] `src/base/hardware/detectors/NetworkDetector.cpp`
- [ ] `src/base/hardware/platform/LinuxDetector.cpp`
- [ ] `src/base/hardware/platform/WindowsDetector.cpp`

### 测试文件
- [ ] `tests/unit/base/hardware/test_hardware_probe.cpp`
- [ ] `tests/unit/base/hardware/test_capability_policy.cpp`
- [ ] `tests/mock/proc/cpuinfo_imx6ull`
- [ ] `tests/mock/proc/cpuinfo_rk3568`
- [ ] `tests/mock/proc/cpuinfo_rk3588`
- [ ] `tests/mock/proc/meminfo_512mb`
- [ ] `tests/mock/proc/meminfo_1gb`
- [ ] `tests/mock/proc/meminfo_4gb`

---

## 七、相关文档

- 设计文档: [../design_stage/01_phase1_hardware_probe.md](../design_stage/01_phase1_hardware_probe.md)
- 依赖: [00_project_skeleton.md](00_project_skeleton.md)

---

*最后更新: 2026-03-05*
