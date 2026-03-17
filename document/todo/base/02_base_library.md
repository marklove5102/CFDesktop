# Phase 2: Base 库核心 TODO

> **状态**: 🚧 进行中 (~30%)
> **预计周期**: 3~4 周
> **依赖阶段**: Phase 0, Phase 1
> **目标交付物**: 配置中心、日志系统

> **已完成归档**: [`done/02_base_library_partial_done.md`](done/02_base_library_partial_done.md) (主题引擎、动画管理器、DPI适配)

> **架构说明**: 主题引擎、动画管理器、DPI 适配已在 `ui/` 目录实现

---

## 一、阶段目标

### 核心目标
建立所有上层模块依赖的基础设施，包括配置中心和日志系统。

### 具体交付物
- [x] `ThemeEngine` 主题引擎模块 (在 ui/core/) ✅ 归档
- [x] `AnimationManager` 动画管理器 (在 ui/components/) ✅ 归档
- [x] `DPIManager` 分辨率适配管理器 (在 ui/base/) ✅ 归档
- [ ] `ConfigStore` 配置中心
- [x] `Logger` 日志系统
- [x] 各模块单元测试 (部分) ✅ 归档

---

## 二、待实现任务

### Week 3: 配置中心

#### Day 3-4: ConfigStore 实现
- [ ] 创建 ConfigStore 类
  - [ ] 三层存储 (System/User/App)
  - [ ] 命名空间支持
- [ ] 实现层级存储
  - [ ] 点分隔键名
  - [ ] 优先级处理
- [ ] 实现变更监听
  - [ ] `watch()` 方法
  - [ ] 信号触发
- [ ] 实现配置持久化
  - [ ] QSettings 包装
  - [ ] 文件格式
- [ ] 编写配置测试

#### Day 5: 测试
- [ ] 跨平台测试
  - [ ] Linux 测试
  - [ ] Windows 测试
- [ ] 单元测试
  - [ ] 配置存储测试
- [ ] 集成测试

---

## 三、验收标准

### 配置中心
- [ ] 层级存储正常
- [ ] 变更监听触发
- [ ] 持久化正常

### 日志系统
- [ ] 多 Sink 并发安全
- [ ] 日志轮转正常
- [ ] 性能影响 < 1%

---

## 四、文件清单 (待实现)

### ConfigStore
- [ ] `include/CFDesktop/Base/ConfigStore/ConfigStore.h`
- [ ] `include/CFDesktop/Base/ConfigStore/ConfigNode.h`
- [ ] `include/CFDesktop/Base/ConfigStore/ConfigWatcher.h`
- [ ] `src/base/config/ConfigStore.cpp`
- [ ] `src/base/config/ConfigWatcher.cpp`

### Logger
- [ ] `include/CFDesktop/Base/Logger/Logger.h`
- [ ] `include/CFDesktop/Base/Logger/LogMessage.h`
- [ ] `include/CFDesktop/Base/Logger/LogSink.h`
- [ ] `include/CFDesktop/Base/Logger/FileSink.h`
- [ ] `include/CFDesktop/Base/Logger/ConsoleSink.h`
- [ ] `include/CFDesktop/Base/Logger/NetworkSink.h`
- [ ] `src/base/logging/Logger.cpp`
- [ ] `src/base/logging/FileSink.cpp`
- [ ] `src/base/logging/ConsoleSink.cpp`
- [ ] `src/base/logging/NetworkSink.cpp`

### 资源文件
- [ ] `configs/logging.conf`

---

## 五、相关文档

- 设计文档: [../design_stage/02_phase2_base_library.md](../design_stage/02_phase2_base_library.md)
- 完成归档: [done/02_base_library_partial_done.md](done/02_base_library_partial_done.md)
- 依赖: [00_project_skeleton.md](00_project_skeleton.md), [01_hardware_probe.md](01_hardware_probe.md)

---

*最后更新: 2026-03-11*
