# Phase 2: Base 库核心 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 3~4 周
> **依赖阶段**: Phase 0, Phase 1
> **目标交付物**: 主题引擎、动画管理器、DPI 适配、配置中心、日志系统

---

## 一、阶段目标

### 核心目标
建立所有上层模块依赖的基础设施，包括主题引擎、动画管理、分辨率适配、配置中心和日志系统。

### 具体交付物
- [ ] `ThemeEngine` 主题引擎模块
- [ ] `AnimationManager` 动画管理器
- [ ] `DPIManager` 分辨率适配管理器
- [ ] `ConfigStore` 配置中心
- [ ] `Logger` 日志系统
- [ ] 各模块单元测试

---

## 二、Week 1: 主题引擎

### Day 1-2: 基础结构
- [ ] 创建 ThemeEngine 类框架
  - [ ] 定义 `Theme` 数据结构
  - [ ] 单例模式实现
  - [ ] 信号定义
- [ ] 定义主题数据结构
  - [ ] `Theme` 主结构
  - [ ] `ColorVariables`
  - [ ] `SizeVariables`
  - [ ] `FontVariables`
- [ ] 实现主题加载器 `ThemeLoader`
  - [ ] JSON 解析
  - [ ] 文件系统扫描
- [ ] 创建默认主题包
  - [ ] `assets/themes/default/theme.json`
  - [ ] `assets/themes/default/variables/colors.json`
  - [ ] `assets/themes/default/variables/sizes.json`
  - [ ] `assets/themes/default/variables/fonts.json`

### Day 3: 变量系统
- [ ] 实现变量解析器 `VariableResolver`
  - [ ] 点分隔路径支持 (如 `text.primary`)
  - [ ] 嵌套对象解析
- [ ] 支持点分隔路径
- [ ] 实现变量继承
  - [ ] 父主题变量覆盖
  - [ ] 缺失变量回退

### Day 4: QSS 处理
- [ ] 实现 QSS 变量替换 `QSSProcessor`
  - [ ] `@variable` 语法解析
  - [ ] 递归替换
- [ ] 实现 QSS 合并
  - [ ] 多文件合并
  - [ ] 优先级处理
- [ ] 支持主题继承
  - [ ] 继承链解析
  - [ ] 循环继承检测

### Day 5: 集成与测试
- [ ] 集成到应用
  - [ ] Application 初始化
  - [ ] QApplication::setStyleSheet()
- [ ] 编写单元测试
  - [ ] 变量查询测试
  - [ ] QSS 处理测试
  - [ ] 继承测试
- [ ] 性能优化
  - [ ] 目标: 热重载 < 100ms

---

## 三、Week 2: 动画管理器

### Day 1-2: 核心功能
- [ ] 创建 AnimationManager 类
  - [ ] 单例模式
  - [ ] 动画列表管理
- [ ] 实现基础动画创建
  - [ ] `createPropertyAnimation()`
  - [ ] 动画生命周期管理
- [ ] 实现 HWTier 降级逻辑
  - [ ] Low 档: 时长归零
  - [ ] Mid 档: 简化动画
  - [ ] High 档: 完整动画
- [ ] 实现 `adjustDuration()` 方法

### Day 3: 预定义动画
- [ ] 实现淡入淡出
  - [ ] `createFadeIn()`
  - [ ] `createFadeOut()`
- [ ] 实现滑动动画
  - [ ] `createSlideIn()`
  - [ ] 方向支持
- [ ] 实现缩放动画
  - [ ] `createScale()`
- [ ] 实现旋转动画
  - [ ] `createRotation()`

### Day 4: 组动画
- [ ] 实现并行动画组
  - [ ] `createParallelGroup()`
  - [ ] 同步播放
- [ ] 实现串行动画组
  - [ ] `createSequentialGroup()`
  - [ ] 顺序播放
- [ ] 实现动画生命周期管理
  - [ ] 启动/暂停/停止
  - [ ] 清理机制

### Day 5: 测试与优化
- [ ] 编写单元测试
  - [ ] 各动画类型测试
  - [ ] 降级逻辑测试
- [ ] 性能测试
  - [ ] 帧率测试
  - [ ] CPU 占用
- [ ] 内存泄漏检查
  - [ ] Valgrind 检查
  - [ ] 动画对象清理

---

## 四、Week 3: DPI 管理器与配置中心

### Day 1-2: DPI 管理器
- [ ] 创建 DPIManager 类
  - [ ] 单例模式
  - [ ] 屏幕参数检测
- [ ] 实现屏幕检测
  - [ ] QScreen 查询
  - [ ] 物理尺寸计算
- [ ] 实现 dp/sp 转换
  - [ ] `dp()` 方法
  - [ ] `sp()` 方法
  - [ ] 基准 DPI 160
- [ ] 实现模拟器注入接口
  - [ ] `injectScreenParameters()`
  - [ ] `clearInjectedParameters()`

### Day 3-4: 配置中心
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

### Day 5: 测试
- [ ] 跨平台测试
  - [ ] Linux 测试
  - [ ] Windows 测试
- [ ] 单元测试
  - [ ] DPI 转换测试
  - [ ] 配置存储测试
- [ ] 集成测试

---

## 五、Week 4: 日志系统与集成

### Day 1-2: 日志系统
- [ ] 创建 Logger 类
  - [ ] 单例模式
  - [ ] 等级过滤
- [ ] 实现 FileSink
  - [ ] 文件轮转
  - [ ] 异步写入
- [ ] 实现 ConsoleSink
  - [ ] 彩色输出
  - [ ] 格式化
- [ ] 实现 NetworkSink
  - [ ] UDP 发送
  - [ ] 远程日志

### Day 3: 日志增强
- [ ] 实现日志轮转
  - [ ] 大小限制
  - [ ] 文件数量限制
- [ ] 实现标签过滤
  - [ ] 白名单
  - [ ] 黑名单
- [ ] 实现彩色输出
  - [ ] ANSI 颜色码
  - [ ] Windows 支持

### Day 4: 集成测试
- [ ] 全模块集成
  - [ ] Theme + Animation
  - [ ] DPI + Config
- [ ] 端到端测试
  - [ ] 应用启动流程
  - [ ] 配置加载流程
- [ ] 性能测试
  - [ ] 启动时间
  - [ ] 内存占用

### Day 5: 文档与发布
- [ ] API 文档
  - [ ] Doxygen 注释
  - [ ] 使用示例
- [ ] 使用示例
  - [ ] 主题切换
  - [ ] 动画使用
- [ ] Code Review

---

## 六、验收标准

### 主题引擎
- [ ] 支持动态切换主题
- [ ] 支持变量继承
- [ ] Low 档位自动禁用阴影/圆角/渐变
- [ ] 热重载时间 < 100ms

### 动画管理器
- [ ] Low 档位动画时长归零
- [ ] 所有预定义动画正常工作
- [ ] 无内存泄漏
- [ ] 动画不阻塞主线程

### DPI 管理器
- [ ] dp/sp 转换准确
- [ ] 支持模拟器注入
- [ ] 热插拔屏幕自动适配

### 配置中心
- [ ] 层级存储正常
- [ ] 变更监听触发
- [ ] 持久化正常

### 日志系统
- [ ] 多 Sink 并发安全
- [ ] 日志轮转正常
- [ ] 性能影响 < 1%

---

## 七、文件清单

### ThemeEngine
- [ ] `include/CFDesktop/Base/ThemeEngine/ThemeEngine.h`
- [ ] `include/CFDesktop/Base/ThemeEngine/Theme.h`
- [ ] `include/CFDesktop/Base/ThemeEngine/ThemeLoader.h`
- [ ] `src/base/theme/ThemeEngine.cpp`
- [ ] `src/base/theme/ThemeLoader.cpp`
- [ ] `src/base/theme/QSSProcessor.cpp`
- [ ] `src/base/theme/VariableResolver.cpp`

### AnimationManager
- [ ] `include/CFDesktop/Base/AnimationManager/AnimationManager.h`
- [ ] `include/CFDesktop/Base/AnimationManager/Animation.h`
- [ ] `include/CFDesktop/Base/AnimationManager/PropertyAnimation.h`
- [ ] `include/CFDesktop/Base/AnimationManager/GroupAnimation.h`
- [ ] `src/base/animation/AnimationManager.cpp`
- [ ] `src/base/animation/PropertyAnimation.cpp`
- [ ] `src/base/animation/GroupAnimation.cpp`

### DPIManager
- [ ] `include/CFDesktop/Base/DPIManager/DPIManager.h`
- [ ] `include/CFDesktop/Base/DPIManager/DisplayInfo.h`
- [ ] `include/CFDesktop/Base/DPIManager/DPIConverter.h`
- [ ] `src/base/dpi/DPIManager.cpp`
- [ ] `src/base/dpi/DPIConverter.cpp`

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
- [ ] `assets/themes/default/theme.json`
- [ ] `assets/themes/default/variables/colors.json`
- [ ] `assets/themes/default/variables/sizes.json`
- [ ] `assets/themes/default/variables/fonts.json`
- [ ] `assets/themes/default/styles/base.qss`
- [ ] `assets/themes/default/styles/widgets.qss`
- [ ] `assets/themes/default/styles/animations.qss`
- [ ] `configs/logging.conf`

---

## 八、相关文档

- 设计文档: [../design_stage/02_phase2_base_library.md](../design_stage/02_phase2_base_library.md)
- 依赖: [00_project_skeleton.md](00_project_skeleton.md), [01_hardware_probe.md](01_hardware_probe.md)

---

*最后更新: 2026-03-05*
