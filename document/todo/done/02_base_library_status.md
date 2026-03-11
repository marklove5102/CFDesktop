# Phase 2: 基础库核心 - 状态文档

> **模块ID**: Phase 2
> **状态**: 🚧 部分完成
> **总体进度**: 35%
> **最后更新**: 2026-03-11
> **架构说明**: 原规划在 `base/` 目录，实际实现在 `ui/` 目录

---

## 一、模块概述

Base库核心是CFDesktop项目的基础设施层，为所有上层模块提供统一的主题管理、动画控制、分辨率适配、配置存储和日志记录能力。

### 核心职责

1. **主题管理** - 动态主题切换、变量解析、QSS处理
2. **动画管理** - 动画生命周期、性能降级、预定义动画
3. **分辨率适配** - dp/sp单位转换、屏幕参数检测
4. **配置中心** - 层级存储、变更监听、持久化
5. **日志系统** - 多Sink支持、日志轮转、标签过滤

---

## 二、完成状态总览

| 模块 | 完成度 | 实现位置 | 备注 |
|------|--------|----------|------|
| ThemeEngine | 60% | `ui/core/` | 核心框架已实现 |
| AnimationManager | 65% | `ui/components/` | 接口已定义 |
| DPI management | 80% | `ui/base/` | 基础功能完成 |
| ConfigStore | 0% | - | 未实现 |
| Logger | 0% | - | 未实现 |

---

## 三、已完成工作

### 3.1 主题引擎 ThemeEngine (60%)

**实际实现位置**: `ui/core/` (非原计划的 `base/`)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`ui/core/theme.h`](../../../ui/core/theme.h) | ICFTheme 接口 |
| [`ui/core/theme_manager.h`](../../../ui/core/theme_manager.h) | 主题管理器 (单例) |
| [`ui/core/theme_factory.h`](../../../ui/core/theme_factory.h) | 主题工厂接口 |
| [`ui/core/material/cfmaterial_theme.h`](../../../ui/core/material/cfmaterial_theme.h) | Material 主题实现 |
| [`ui/core/material/cfmaterial_scheme.h`](../../../ui/core/material/cfmaterial_scheme.h) | Material 配色方案 |
| [`ui/core/material/cfmaterial_radius_scale.h`](../../../ui/core/material/cfmaterial_radius_scale.h) | 圆角规范 |
| [`ui/core/material/cfmaterial_fonttype.h`](../../../ui/core/material/cfmaterial_fonttype.h) | 字体规范 |
| [`ui/core/material/cfmaterial_motion.h`](../../../ui/core/material/cfmaterial_motion.h) | 动画规范 |
| [`ui/core/material/material_factory_class.h`](../../../ui/core/material/material_factory_class.h) | Material 工厂 |

#### 功能实现

- [x] 主题管理器 - `ThemeManager` 单例
- [x] 主题注册/卸载 - `insert_one()`, `remove_one()`
- [x] 主题切换 - `setThemeTo()`
- [x] 主题变更信号 - `themeChanged`
- [x] Widget 主题安装 - `install_widget()`
- [x] Material 配色方案 - MaterialColorScheme
- [x] Token 系统 - `cfmaterial_token_literals.h`
- [x] 工厂模式创建 - MaterialFactory

#### 接口摘要

```cpp
namespace cf::ui::core {
class ThemeManager : public QObject {
public:
    static ThemeManager& instance();
    const ICFTheme& theme(const std::string& name) const;
    bool insert_one(const std::string& name, InstallerMaker make_one);
    void remove_one(const std::string& name);
    void install_widget(QWidget* w);
    void remove_widget(QWidget* w);
    void setThemeTo(const std::string& name, bool doBroadcast = true);
    const std::string& currentThemeName() const;
signals:
    void themeChanged(const ICFTheme& new_theme);
};
}
```

#### 测试

- [x] [`test/ui/core/token_test.cpp`](../../../test/ui/core/token_test.cpp)

#### 示例

- [x] [`example/gui/theme/`](../../../example/gui/theme/) - 主题展示

---

### 3.2 动画管理器 AnimationManager (65%)

**实际实现位置**: `ui/components/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`ui/components/animation.h`](../../../ui/components/animation.h) | 动画基类 |
| [`ui/components/animation_factory_manager.h`](../../../ui/components/animation_factory_manager.h) | 动画工厂管理器 |
| [`ui/components/animation_group.h`](../../../ui/components/animation_group.h) | 动画组 |
| [`ui/components/timing_animation.h`](../../../ui/components/timing_animation.h) | 时序动画 |
| [`ui/components/spring_animation.h`](../../../ui/components/spring_animation.h) | 弹簧动画 |
| [`ui/components/material/cfmaterial_animation_strategy.h`](../../../ui/components/material/cfmaterial_animation_strategy.h) | 动画策略 |
| [`ui/components/material/cfmaterial_animation_factory.h`](../../../ui/components/material/cfmaterial_animation_factory.h) | Material 动画工厂 |
| [`ui/components/material/cfmaterial_fade_animation.h`](../../../ui/components/material/cfmaterial_fade_animation.h) | 淡入淡出 |
| [`ui/components/material/cfmaterial_slide_animation.h`](../../../ui/components/material/cfmaterial_slide_animation.h) | 滑动动画 |
| [`ui/components/material/cfmaterial_scale_animation.h`](../../../ui/components/material/cfmaterial_scale_animation.h) | 缩放动画 |
| [`ui/components/material/cfmaterial_property_animation.h`](../../../ui/components/material/cfmaterial_property_animation.h) | 属性动画 |

#### 功能实现

- [x] 动画工厂管理器
- [x] 动画注册/创建
- [x] 预定义动画:
  - [x] FadeAnimation (淡入淡出)
  - [x] SlideAnimation (滑动，支持方向)
  - [x] ScaleAnimation (缩放)
- [x] 动画组:
  - [x] 并行组
  - [x] 串行组
- [x] 动画生命周期管理
- [x] 动画策略模式
- [x] WeakPtr 所有权管理

#### 核心类型

```cpp
namespace cf::ui::components {
// 动画创建器函数类型
using AnimationCreator = std::function<ICFAbstractAnimation*(QObject*)>;

// 动画工厂管理器接口
class ICFAnimationManagerFactory : public QObject {
    virtual RegisteredResult registerOneAnimation(const QString& name, const QString& type) = 0;
    virtual RegisteredResult registerAnimationCreator(const QString& name, AnimationCreator creator) = 0;
    virtual cf::WeakPtr<ICFAbstractAnimation> getAnimation(const char* name) = 0;
    virtual void setEnabledAll(bool enabled) = 0;
    virtual bool isAllEnabled() = 0;
};

// 动画状态枚举
enum class State { Idle, Running, Paused, Finished };
enum class Direction { Forward, Backward };
}
```

#### 示例

- [x] [`example/gui/material/material_motion_spec/`](../../../example/gui/material/material_motion_spec/) - 动画展示

---

### 3.3 DPI 适配 (80%)

**实际实现位置**: `ui/base/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`ui/base/device_pixel.h`](../../../ui/base/device_pixel.h) | dp/sp 转换工具 |
| [`ui/base/device_pixel.cpp`](../../../ui/base/device_pixel.cpp) | 实现 |

#### 功能实现

- [x] `CanvasUnitHelper` 结构体
- [x] `dpToPx()` - 设备无关像素转物理像素
- [x] `spToPx()` - 可缩放像素转物理像素
- [x] `pxToDp()` - 物理像素转设备无关像素
- [x] `dpi()` - 获取设备像素比
- [x] `BreakPoint` - 响应式断点 (Compact/Medium/Expanded)

#### 接口摘要

```cpp
namespace cf::ui::base::device {
struct CanvasUnitHelper {
    CanvasUnitHelper(const qreal devicePixelRatio);
    qreal dpToPx(qreal dp) const;
    qreal spToPx(qreal sp) const;
    qreal pxToDp(qreal px) const;
    qreal dpi() const;

    enum class BreakPoint { Compact, Medium, Expanded };
    BreakPoint breakPoint(qreal widthDp);
};
}
```

#### 测试

- [x] [`test/ui/base/device_pixel_test.cpp`](../../../test/ui/base/device_pixel_test.cpp)

---

### 3.4 颜色系统支持

**位置**: `ui/base/color.h`, `ui/base/color_helper.h`

**已完成**:
- HCT颜色空间支持 `CFColor` 类
- 颜色混合 `blend()`
- 对比度计算 `contrastRatio()`
- 色调板生成 `tonalPalette()`
- 高程叠加 `elevationOverlay()`
- 相对亮度计算 `relativeLuminance()`

---

## 四、使用示例

### 主题切换

```cpp
#include <ui/core/theme_manager.h>

auto& manager = cf::ui::core::ThemeManager::instance();
manager.setThemeTo("theme.material.light");
```

### 动画使用

```cpp
#include <ui/components/material/cfmaterial_animation_factory.h>

auto factory = std::make_unique<MaterialAnimationFactory>(theme);
auto anim = factory->getAnimation("md.animation.fadeIn");
if (anim) {
    anim->start();
}
```

### DPI 转换

```cpp
#include <ui/base/device_pixel.h>

cf::ui::base::device::CanvasUnitHelper helper(2.0);
qreal pixels = helper.dpToPx(16.0);  // 16dp -> 32px
```

---

## 五、待完成模块

### 5.1 ConfigStore - 配置中心 (0%)

**需求描述**:
- 三层存储 (System/User/App)
- 点分隔键名支持
- 变更监听机制
- 持久化存储

**建议文件路径**:
- `include/CFDesktop/Base/ConfigStore/ConfigStore.h`
- `src/base/config/ConfigStore.cpp`

**关键接口设计**:
```cpp
// 伪代码，待实现
class ConfigStore {
public:
    static ConfigStore& instance();

    // 配置读写
    QVariant get(const QString& key, const QVariant& defaultValue = QVariant());
    void set(const QString& key, const QVariant& value);

    // 层级存储
    void setNamespace(Namespace ns);  // System/User/App

    // 变更监听
    void watch(const QString& key, std::function<void(const QVariant&)> callback);
    void unwatch(const QString& key);

    // 持久化
    void save();
    void load();
};
```

### 5.2 Logger - 日志系统 (0%)

**需求描述**:
- 多等级日志 (Debug/Info/Warning/Error/Fatal)
- 多Sink支持 (File/Console/Network)
- 日志轮转 (大小/数量限制)
- 标签过滤
- 异步写入

**建议文件路径**:
- `include/CFDesktop/Base/Logger/Logger.h`
- `include/CFDesktop/Base/Logger/LogSink.h`
- `include/CFDesktop/Base/Logger/FileSink.h`
- `include/CFDesktop/Base/Logger/ConsoleSink.h`
- `include/CFDesktop/Base/Logger/NetworkSink.h`

**关键接口设计**:
```cpp
// 伪代码，待实现
enum class LogLevel { Debug, Info, Warning, Error, Fatal };

class Logger {
public:
    static Logger& instance();

    void log(LogLevel level, const QString& tag, const QString& message);
    void addSink(std::unique_ptr<LogSink> sink);
    void setTagFilter(const QStringList& whitelist, const QStringList& blacklist);
};

class LogSink {
public:
    virtual void write(const LogMessage& message) = 0;
};
```

### 5.3 其他待完成项

| 模块 | 优先级 | 依赖 |
|------|--------|------|
| QSSProcessor | P1 | - |
| VariableResolver | P1 | - |
| HWTier 降级逻辑 | P0 | HWTier |
| 屏幕参数检测 | P1 | - |
| 模拟器注入接口 | P1 | - |

#### 主题加载器 (部分完成)

**待完成**:
- JSON主题文件解析
- 文件系统扫描
- 主题继承链解析
- 循环继承检测
- QSS变量替换

**建议文件路径**:
- `ui/core/theme_loader.h` (新建)
- `ui/core/qss_processor.h` (新建)

#### 变量解析器 (部分完成)

**待完成**:
- 点分隔路径解析 (如 `text.primary`)
- 嵌套对象解析
- 变量继承和回退

**建议文件路径**:
- `ui/core/variable_resolver.h` (新建)

---

## 六、文件结构总览

### 已实现文件 (ui/ 目录)

```
ui/
├── core/
│   ├── theme_manager.h         # 主题管理器 (核心)
│   ├── theme.h                 # ICFTheme接口
│   ├── theme_factory.h         # ThemeFactory抽象接口
│   ├── color_scheme.h          # ICFColorScheme颜色方案
│   ├── motion_spec.h           # IMotionSpec动画规范
│   ├── font_type.h             # IFontType字体接口
│   └── radius_scale.h          # IRadiusScale圆角接口
├── components/
│   ├── animation_factory_manager.h  # 动画工厂管理器
│   ├── animation.h             # ICFAbstractAnimation基础接口
│   ├── spring_animation.h      # 弹簧动画
│   ├── timing_animation.h      # 时间曲线动画
│   └── animation_group.h       # 动画组
└── base/
    ├── device_pixel.h          # DPI转换工具
    ├── color.h                 # CFColor (HCT颜色)
    ├── color_helper.h          # 颜色工具函数
    ├── easing.h                # 缓动曲线
    ├── geometry_helper.h       # 几何工具
    └── math_helper.h           # 数学工具
```

### 待创建文件 (base/ 目录)

```
include/CFDesktop/Base/
├── ConfigStore/
│   ├── ConfigStore.h          # 配置中心
│   ├── ConfigNode.h           # 配置节点
│   └── ConfigWatcher.h        # 配置监听器
└── Logger/
    ├── Logger.h               # 日志系统
    ├── LogMessage.h           # 日志消息
    ├── LogSink.h              # Sink基类
    ├── FileSink.h             # 文件Sink
    ├── ConsoleSink.h          # 控制台Sink
    └── NetworkSink.h          # 网络Sink

src/base/
├── config/
│   ├── ConfigStore.cpp
│   └── ConfigWatcher.cpp
└── logging/
    ├── Logger.cpp
    ├── FileSink.cpp
    ├── ConsoleSink.cpp
    └── NetworkSink.cpp

ui/core/
├── theme_loader.h             # 主题加载器 (待创建)
├── qss_processor.h            # QSS处理器 (待创建)
└── variable_resolver.h        # 变量解析器 (待创建)
```

---

## 七、下一步行动建议

### 优先级1 (高)

1. **实现ConfigStore配置中心**
   - 优先级: 最高
   - 理由: 其他模块可能依赖配置能力
   - 预计工作量: 3-4天

2. **实现Logger日志系统**
   - 优先级: 高
   - 理由: 调试和问题追踪需要
   - 预计工作量: 3-4天

### 优先级2 (中)

3. **完善主题加载器**
   - 实现JSON主题文件解析
   - 实现主题继承
   - 预计工作量: 2-3天

4. **完善变量解析器**
   - 实现点分隔路径解析
   - 实现变量回退机制
   - 预计工作量: 1-2天

### 优先级3 (低)

5. **增强DPI管理**
   - 添加模拟器注入接口
   - 支持热插拔屏幕
   - 预计工作量: 1-2天

6. **动画性能优化**
   - 实现HWTier降级逻辑
   - 添加性能测试
   - 预计工作量: 2-3天

---

## 八、相关文档

- 原始TODO: [../02_base_library.md](../02_base_library.md)
- 设计文档: [../../design_stage/02_phase2_base_library.md](../../design_stage/02_phase2_base_library.md)

---

*文档版本: v1.0*
*生成时间: 2026-03-11*
