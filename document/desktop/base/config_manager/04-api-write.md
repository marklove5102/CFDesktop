# ConfigStore API - 写入操作

本文档介绍 ConfigStore 的配置写入 API。

## 目录

- [set() - 设置配置值](#set---设置配置值)
- [register_key() - 注册配置键](#register_key---注册配置键)
- [unregister_key() - 注销配置键](#unregister_key---注销配置键)
- [clear() - 清空所有配置](#clear---清空所有配置)
- [clear_layer() - 清空指定层](#clear_layer---清空指定层)

---

## set() - 设置配置值

设置配置键的值。

### 函数签名

```cpp
template <typename Value>
[[nodiscard]] bool set(
    const KeyView key,
    const Value& v,
    Layer layer = Layer::App,
    NotifyPolicy notify_policy = NotifyPolicy::Immediate
);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `KeyView` | 配置键视图 |
| `v` | `const Value&` | 要设置的值 |
| `layer` | `Layer` | 目标层级，默认 `Layer::App` |
| `notify_policy` | `NotifyPolicy` | 通知策略，默认 `Immediate` |

### 返回值

| 类型 | 说明 |
|------|------|
| `bool` | 成功返回 `true`，失败返回 `false` |

### 写入流程

```
set() 执行流程:
   KeyView + Value
        ↓
   ┌─────────────────────────────┐
   │  1. KeyView 转换为 Key      │
   │  2. 值转换为 std::any        │
   │  3. 写入指定 Layer          │
   │  4. 根据 NotifyPolicy 处理   │
   │                             │
   │  Manual:    标记待通知       │
   │  Immediate: 立即触发 Watcher │
   └─────────────────────────────┘
        ↓
   返回 bool
```

### 使用示例

```cpp
using namespace cf::config;

// 基本使用 - 设置字符串
bool success = ConfigStore::instance().set(
    KeyView{.group = "app.theme", .key = "name"},
    std::string("dark"),
    Layer::App
);

// 设置数值
ConfigStore::instance().set(
    KeyView{.group = "app.window", .key = "width"},
    1920,
    Layer::App
);

// 设置布尔值
ConfigStore::instance().set(
    KeyView{.group = "app.window", .key = "fullscreen"},
    true,
    Layer::User
);

// 写入不同层级
ConfigStore::instance().set(
    KeyView{.group = "test", .key = "timeout"},
    5000,
    Layer::Temp  // 临时层，不持久化
);

// 使用手动通知策略 - 批量修改
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::App,
    NotifyPolicy::Manual
);

ConfigStore::instance().set(
    KeyView{.group = "app", .key = "language"},
    std::string("zh-CN"),
    Layer::App,
    NotifyPolicy::Manual
);

// 统一触发通知
ConfigStore::instance().notify();
```

---

## register_key() - 注册配置键

显式注册配置键，用于配置项的声明和文档生成。

### 函数签名

```cpp
template <typename Value>
[[nodiscard]] RegisterResult register_key(
    const Key& key,
    const Value& init_value,
    Layer layer = Layer::App,
    NotifyPolicy notify_policy = NotifyPolicy::Immediate
);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `const Key&` | 完整键定义（含描述） |
| `init_value` | `const Value&` | 初始值 |
| `layer` | `Layer` | 目标层级，默认 `Layer::App` |
| `notify_policy` | `NotifyPolicy` | 通知策略，默认 `Immediate` |

### 返回值

| 值 | 说明 |
|------|------|
| `RegisterResult::KeyAlreadyIn` | 键已存在，注册失败 |
| `RegisterResult::KeyRegisteredSuccess` | 注册成功 |

### 注册与 set 的区别

| 操作 | 键定义 | 描述 | 典型用途 |
|------|--------|------|----------|
| `register_key()` | 需要 `Key` (含描述) | 显式声明，带初始值 | 配置项初始化、文档生成 |
| `set()` | 仅需 `KeyView` | 直接设置值 | 运行时配置修改 |

### 使用示例

```cpp
using namespace cf::config;

// 注册主题配置
Key theme_key{
    .full_key = "app.theme.name",
    .full_description = "Application theme name (default, light, dark)"
};

auto result = ConfigStore::instance().register_key(
    theme_key,
    std::string("default"),
    Layer::App
);

if (result == RegisterResult::KeyRegisteredSuccess) {
    std::cout << "Theme key registered" << std::endl;
} else {
    std::cout << "Theme key already exists" << std::endl;
}

// 注册窗口配置
Key window_width{
    .full_key = "app.window.width",
    .full_description = "Main window width in pixels"
};

ConfigStore::instance().register_key(
    window_width,
    1280,
    Layer::App,
    NotifyPolicy::Manual
);

// 注册系统默认配置
Key system_lang{
    .full_key = "app.language",
    .full_description = "System default language code"
};

ConfigStore::instance().register_key(
    system_lang,
    std::string("en-US"),
    Layer::System
);

// 注册到用户层（用户偏好）
Key user_font_size{
    .full_key = "app.ui.font_size",
    .full_description = "User preferred font size"
};

ConfigStore::instance().register_key(
    user_font_size,
    14,
    Layer::User
);
```

---

## unregister_key() - 注销配置键

注销已注册的配置键。

### 函数签名

```cpp
[[nodiscard]] UnRegisterResult unregister_key(
    const Key& key,
    Layer layer = Layer::App,
    NotifyPolicy notify_policy = NotifyPolicy::Immediate
);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `const Key&` | 要注销的键 |
| `layer` | `Layer` | 目标层级，默认 `Layer::App` |
| `notify_policy` | `NotifyPolicy` | 通知策略，默认 `Immediate` |

### 返回值

| 值 | 说明 |
|------|------|
| `UnRegisterResult::KeyUnexisted` | 键不存在，注销失败 |
| `UnRegisterResult::KeyUnRegisteredSuccess` | 注销成功 |

### 使用示例

```cpp
using namespace cf::config;

// 注销配置键
Key old_key{
    .full_key = "app.deprecated_feature",
    .full_description = "Deprecated feature flag"
};

auto result = ConfigStore::instance().unregister_key(
    old_key,
    Layer::App
);

if (result == UnRegisterResult::KeyUnRegisteredSuccess) {
    std::cout << "Key unregistered" << std::endl;
} else {
    std::cout << "Key not found" << std::endl;
}

// 从特定层注销
auto user_result = ConfigStore::instance().unregister_key(
    Key{.full_key = "app.temp.setting", .full_description = ""},
    Layer::User,
    NotifyPolicy::Manual
);
```

---

## clear() - 清空所有配置

清空所有层的所有配置。

### 函数签名

```cpp
void clear();
```

### 警告

> 此操作不可逆，请谨慎使用。

### 使用示例

```cpp
using namespace cf::config;

// 重置所有配置（测试场景）
ConfigStore::instance().clear();

// 重新初始化默认配置
initialize_default_config();
```

---

## clear_layer() - 清空指定层

清空指定层的所有配置。

### 函数签名

```cpp
void clear_layer(Layer layer);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `layer` | `Layer` | 要清空的层级 |

### 使用示例

```cpp
using namespace cf::config;

// 清空临时层（测试清理）
ConfigStore::instance().clear_layer(Layer::Temp);

// 重置用户配置
ConfigStore::instance().clear_layer(Layer::User);
// 用户登出时清除个人设置

// 清空应用层配置
ConfigStore::instance().clear_layer(Layer::App);
// 应用重置为默认状态
```

### 清空范围对比

```
clear() 影响范围:
   ┌───────────────────────────┐
   │  [Temp]  ✓ 清空            │
   │  [App]   ✓ 清空            │
   │  [User]  ✓ 清空            │
   │  [System] ✓ 清空           │
   └───────────────────────────┘

clear_layer(Layer::App) 影响范围:
   ┌───────────────────────────┐
   │  [Temp]  × 保留            │
   │  [App]   ✓ 清空            │
   │  [User]  × 保留            │
   │  [System] × 保留           │
   └───────────────────────────┘
```

---

## 写入操作与通知

### 通知策略对比

| 策略 | 行为 | 使用场景 |
|------|------|----------|
| `NotifyPolicy::Manual` | 变更不触发回调 | 批量修改、避免频繁触发 |
| `NotifyPolicy::Immediate` | 立即触发回调 | 需要即时响应 |

### 批量写入模式

```cpp
using namespace cf::config;

// 模式1: 使用 Manual 策略批量修改
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::App,
    NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "language"},
    std::string("zh-CN"),
    Layer::App,
    NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app.ui", .key = "font_size"},
    14,
    Layer::App,
    NotifyPolicy::Manual
);
ConfigStore::instance().notify();  // 统一触发通知

// 模式2: 使用 Temp 层暂存，完成后移动到 App
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::Temp
);
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "language"},
    std::string("zh-CN"),
    Layer::Temp
);
// ... 验证配置 ...
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::App
);
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "language"},
    std::string("zh-CN"),
    Layer::App
);
ConfigStore::instance().clear_layer(Layer::Temp);
```

---

## 下一章

- [05-api-watch.md](./05-api-watch.md) - 监听操作 API
