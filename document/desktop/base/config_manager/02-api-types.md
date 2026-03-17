# ConfigStore API - 核心类型

本文档介绍 ConfigStore API 中使用的核心数据类型。

## 目录

- [Layer 枚举](#layer-枚举)
- [NotifyPolicy 枚举](#notifypolicy-枚举)
- [KeyView 结构体](#keyview-结构体)
- [Key 结构体](#key-结构体)
- [Watcher 回调类型](#watcher-回调类型)
- [辅助类型](#辅助类型)

---

## Layer 枚举

配置存储层，定义配置的优先级和持久化策略。

### 定义

```cpp
namespace cf::config {
enum class Layer { System, User, App, Temp };
}
```

### 层级说明

| 层级 | 优先级 | 持久化 | 说明 |
|------|--------|--------|------|
| `Temp` | 最高 | 否 | 临时层，仅内存存储，用于测试或临时配置变更 |
| `App` | 高 | 是 | 应用层，应用级别的配置设置 |
| `User` | 中 | 是 | 用户层，用户个人配置偏好 |
| `System` | 低 | 是 | 系统层，系统默认配置和回退值 |

### 查询优先级

```
查询顺序: Temp -> App -> User -> System
   |         |       |        |
   v         v       v        v
 [最高]  [高]   [中]    [低/默认]
```

### 使用示例

```cpp
using namespace cf::config;

// 查询时按优先级自动查找
std::string theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"},
    "default"
);

// 从特定层查询
auto user_theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"},
    Layer::User
);
if (user_theme.has_value()) {
    // 使用 user_theme.value()
}

// 写入指定层
ConfigStore::instance().set(
    KeyView{.group = "app.theme", .key = "name"},
    std::string("dark"),
    Layer::App
);
```

---

## NotifyPolicy 枚举

通知策略，控制配置变更时是否立即触发监听回调。

### 定义

```cpp
namespace cf::config {
enum class NotifyPolicy { Manual, Immediate };
}
```

### 策略说明

| 策略 | 行为 | 使用场景 |
|------|------|----------|
| `Manual` | 手动通知，变更不触发回调，需调用 `notify()` | 批量修改配置，避免多次触发 |
| `Immediate` | 立即通知，每次变更立即触发回调 | 需要即时响应配置变化 |

### 使用示例

```cpp
using namespace cf::config;

// 手动通知模式 - 批量修改
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

// 立即通知模式 - 即时响应
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "volume"},
    80,
    Layer::App,
    NotifyPolicy::Immediate  // 立即触发 Watcher 回调
);
```

---

## KeyView 结构体

配置键的轻量级视图，用于运行时配置访问。

### 定义

```cpp
namespace cf::config {
struct KeyView {
    std::string_view group;
    std::string_view key;
};
}
```

### 字段说明

| 字段 | 类型 | 说明 |
|------|------|------|
| `group` | `std::string_view` | 配置分组，如 `"app.theme"` |
| `key` | `std::string_view` | 配置键名，如 `"name"` |

### 组合规则

```
KeyView{group = "app.theme", key = "name"}
        ↓
    完整键: "app.theme.name"
```

### 使用示例

```cpp
using namespace cf::config;

// 查询配置
KeyView theme_key{.group = "app.theme", .key = "name"};
std::string theme = ConfigStore::instance().query<std::string>(
    theme_key,
    "default"
);

// 修改配置
ConfigStore::instance().set(
    KeyView{.group = "app.window", .key = "width"},
    1920,
    Layer::App
);

// 检查键是否存在
bool has = ConfigStore::instance().has_key(
    KeyView{.group = "app", .key = "version"}
);
```

---

## Key 结构体

配置键的完整定义，用于键注册和持久化描述。

### 定义

```cpp
namespace cf::config {
struct Key {
    std::string full_key;        // 完整键路径 "app.theme.name"
    std::string full_description; // 配置项描述，用于生成帮助文档
};
}
```

### 字段说明

| 字段 | 类型 | 说明 |
|------|------|------|
| `full_key` | `std::string` | 完整键路径，由 `group.key` 组合而成 |
| `full_description` | `std::string` | 配置项的详细描述 |

### 与 KeyView 的关系

```
KeyView         KeyHelper          Key
  ↓                 ↓                ↓
[group="a.b"]   转换器    [full_key="a.b.c"]
[key="c"]                    [description="..."]
```

### 使用示例

```cpp
using namespace cf::config;

// 注册配置键
Key theme_key{
    .full_key = "app.theme.name",
    .full_description = "Application theme name (default, light, dark)"
};

ConfigStore::instance().register_key(
    theme_key,
    std::string("default"),
    Layer::App
);

// Watcher 回调中接收完整 Key
WatcherHandle handle = ConfigStore::instance().watch(
    "app.theme.*",
    [](const Key& k, const std::any* old_value, const std::any* new_value, Layer from_layer) {
        // k.full_key 和 k.full_description 可用
    }
);
```

---

## Watcher 回调类型

配置变更监听器回调函数类型。

### 定义

```cpp
namespace cf::config {
using Watcher = std::function<void(
    const Key& k,
    const std::any* old_value,
    const std::any* new_value,
    Layer from_layer
)>;
}
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `k` | `const Key&` | 发生变更的配置键 |
| `old_value` | `const std::any*` | 旧值指针，新增键时为 `nullptr` |
| `new_value` | `const std::any*` | 新值指针，删除键时为 `nullptr` |
| `from_layer` | `Layer` | 变更发生的层级 |

### 注意事项

- `old_value` 和 `new_value` 仅在回调执行期间有效
- 回调中避免调用 `ConfigStore::set()` 以防止死锁
- 使用 `std::any_cast` 转换值类型

### 使用示例

```cpp
using namespace cf::config;

WatcherHandle handle = ConfigStore::instance().watch(
    "app.theme.*",
    [](const Key& k, const std::any* old_value, const std::any* new_value, Layer from_layer) {
        if (new_value) {
            std::string new_theme = std::any_cast<std::string>(*new_value);
            std::cout << "Theme changed to: " << new_theme << std::endl;
        }

        if (old_value && new_value) {
            std::string old = std::any_cast<std::string>(*old_value);
            std::string neu = std::any_cast<std::string>(*new_value);
            std::cout << "Updated: " << old << " -> " << neu << std::endl;
        }
    },
    NotifyPolicy::Immediate
);
```

---

## 辅助类型

### RegisterResult

键注册结果。

```cpp
enum class RegisterResult {
    KeyAlreadyIn = 0,        // 键已存在
    KeyRegisteredSuccess = 1 // 注册成功
};
```

### UnRegisterResult

键注销结果。

```cpp
enum class UnRegisterResult {
    KeyUnexisted = 0,         // 键不存在
    KeyUnRegisteredSuccess = 1 // 注销成功
};
```

### NotifyResult

通知操作结果。

```cpp
enum class NotifyResult {
    NotifyFailed = 0,        // 通知失败（内部错误）
    NothingWorthNotify = 1,  // 无待通知的变更
    NotifySuccess = 2        // 通知成功
};
```

### WatcherHandle

监听器句柄，用于取消监听。

```cpp
using WatcherHandle = std::size_t;
```

### SyncMethod

同步方法。

```cpp
enum class SyncMethod { Sync, Async };
```

| 值 | 说明 |
|------|------|
| `Sync` | 同步写入，阻塞直到完成 |
| `Async` | 异步写入，立即返回 |

---

## 下一章

- [03-api-query.md](./03-api-query.md) - 查询操作 API
