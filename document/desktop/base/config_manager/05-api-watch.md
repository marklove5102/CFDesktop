# ConfigStore API - 监听操作

本文档介绍 ConfigStore 的配置变更监听 API。

## 目录

- [watch() - 注册监听器](#watch---注册监听器)
- [unwatch() - 取消监听](#unwatch---取消监听)
- [notify() - 手动触发通知](#notify---手动触发通知)
- [pending_changes() - 待通知变更计数](#pending_changes---待通知变更计数)

---

## watch() - 注册监听器

监听配置键的变更，当键值变化时触发回调。

### 函数签名

```cpp
WatcherHandle watch(
    const std::string& key_pattern,
    Watcher callback,
    NotifyPolicy policy = NotifyPolicy::Immediate
);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key_pattern` | `const std::string&` | 键模式，支持通配符 |
| `callback` | `Watcher` | 回调函数 |
| `policy` | `NotifyPolicy` | 通知策略，默认 `Immediate` |

### 返回值

| 类型 | 说明 |
|------|------|
| `WatcherHandle` | 监听器句柄，用于后续取消监听 |

### 键模式匹配

| 模式 | 匹配示例 | 说明 |
|------|----------|------|
| `"app.theme.name"` | 仅 `app.theme.name` | 精确匹配 |
| `"app.theme.*"` | `app.theme.name`, `app.theme.mode` | 单层通配符 |
| `"app.*.name"` | `app.theme.name`, `app.ui.name` | 单层通配符 |
| `"app.**"` | `app` 下所有键 | 递归通配符（如支持） |

### Watcher 回调签名

```cpp
using Watcher = std::function<void(
    const Key& k,           // 变更的键
    const std::any* old_value,  // 旧值（新增时为 nullptr）
    const std::any* new_value,  // 新值（删除时为 nullptr）
    Layer from_layer        // 变更来源层级
)>;
```

### 回调参数含义

```
变更场景            old_value    new_value    说明
──────────────────────────────────────────────────────
新增配置           nullptr      有效指针      新键创建
修改配置           有效指针      有效指针      值更新
删除配置           有效指针      nullptr      键移除
```

### 使用示例

```cpp
using namespace cf::config;

// 示例1: 监听单个键
WatcherHandle h1 = ConfigStore::instance().watch(
    "app.theme.name",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        if (new_val) {
            std::string theme = std::any_cast<std::string>(*new_val);
            std::cout << "Theme changed to: " << theme << std::endl;
            apply_theme(theme);
        }
    }
);

// 示例2: 监听一组键（通配符）
WatcherHandle h2 = ConfigStore::instance().watch(
    "app.theme.*",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        std::cout << "Theme config changed: " << k.full_key << std::endl;
        reload_theme_config();
    }
);

// 示例3: 处理新增/修改/删除
WatcherHandle h3 = ConfigStore::instance().watch(
    "app.window.*",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        if (!old_val && new_val) {
            std::cout << "New window setting: " << k.full_key << std::endl;
        } else if (old_val && new_val) {
            std::cout << "Window setting updated: " << k.full_key << std::endl;
        } else if (old_val && !new_val) {
            std::cout << "Window setting removed: " << k.full_key << std::endl;
        }
    }
);

// 示例4: 使用 Manual 通知策略
WatcherHandle h4 = ConfigStore::instance().watch(
    "app.batch.*",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        // 仅在 notify() 调用时触发
        std::cout << "Batch update completed for: " << k.full_key << std::endl;
    },
    NotifyPolicy::Manual
);
```

### 注意事项

- 回调中避免调用 `ConfigStore::set()` 以防止死锁
- `old_value` 和 `new_value` 仅在回调执行期间有效
- 不要存储这些指针的引用

---

## unwatch() - 取消监听

取消已注册的配置变更监听器。

### 函数签名

```cpp
void unwatch(WatcherHandle handle);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `handle` | `WatcherHandle` | `watch()` 返回的句柄 |

### 使用示例

```cpp
using namespace cf::config;

// 注册监听
WatcherHandle handle = ConfigStore::instance().watch(
    "app.theme.*",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        // 处理变更
    }
);

// 不再需要监听时取消
ConfigStore::instance().unwatch(handle);

// RAII 封装示例
class ThemeWatcher {
  public:
    ThemeWatcher() {
        handle_ = ConfigStore::instance().watch("app.theme.*", callback);
    }
    ~ThemeWatcher() {
        ConfigStore::instance().unwatch(handle_);
    }
  private:
    WatcherHandle handle_;
    Watcher callback = [](const Key& k, const std::any* old, const std::any* neu, Layer) {
        // ...
    };
};
```

---

## notify() - 手动触发通知

手动触发所有 `NotifyPolicy::Manual` 的监听器回调。

### 函数签名

```cpp
[[nodiscard]] NotifyResult notify();
```

### 返回值

| 值 | 说明 |
|------|------|
| `NotifyResult::NotifyFailed` | 通知失败（内部错误） |
| `NotifyResult::NothingWorthNotify` | 无待通知的变更 |
| `NotifyResult::NotifySuccess` | 通知成功 |

### 通知流程

```
notify() 执行流程:
   ┌─────────────────────────────┐
   │  收集所有 Manual Watcher    │
   │  检查是否有待通知变更        │
   │                             │
   │  无变更 → NothingWorthNotify │
   │  有变更 → 触发回调          │
   │           成功 → NotifySuccess │
   │           失败 → NotifyFailed │
   └─────────────────────────────┘
```

### 使用示例

```cpp
using namespace cf::config;

// 注册 Manual 模式监听器
WatcherHandle handle = ConfigStore::instance().watch(
    "app.settings.*",
    [](const Key& k, const std::any* old_val, const std::any* new_val, Layer layer) {
        std::cout << "Settings updated: " << k.full_key << std::endl;
        refresh_settings_ui();
    },
    NotifyPolicy::Manual
);

// 批量修改配置
ConfigStore::instance().set(
    KeyView{.group = "app.settings", .key = "theme"},
    "dark", Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app.settings", .key = "lang"},
    "zh-CN", Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app.settings", .key = "font_size"},
    14, Layer::App, NotifyPolicy::Manual
);

// 统一触发通知
auto result = ConfigStore::instance().notify();

switch (result) {
    case NotifyResult::NotifySuccess:
        std::cout << "All watchers notified" << std::endl;
        break;
    case NotifyResult::NothingWorthNotify:
        std::cout << "No changes to notify" << std::endl;
        break;
    case NotifyResult::NotifyFailed:
        std::cerr << "Notification failed" << std::endl;
        break;
}
```

### 与 Manual 策略配合使用

```cpp
using namespace cf::config;

// 模式1: 批量更新后统一通知
void update_multiple_settings(const Settings& new_settings) {
    ConfigStore::instance().set(
        KeyView{.group = "app.settings", .key = "theme"},
        new_settings.theme, Layer::App, NotifyPolicy::Manual
    );
    ConfigStore::instance().set(
        KeyView{.group = "app.settings", .key = "lang"},
        new_settings.lang, Layer::App, NotifyPolicy::Manual
    );
    ConfigStore::instance().set(
        KeyView{.group = "app.settings", .key = "font"},
        new_settings.font, Layer::App, NotifyPolicy::Manual
    );
    ConfigStore::instance().notify();  // 触发一次回调
}

// 模式2: 延迟通知
void schedule_later_update() {
    ConfigStore::instance().set(
        KeyView{.group = "app", .key = "pending"},
        true, Layer::App, NotifyPolicy::Manual
    );
    // ... 其他操作 ...
}

void commit_updates() {
    ConfigStore::instance().notify();  // 提交变更
}
```

---

## pending_changes() - 待通知变更计数

获取当前待通知的变更数量（用于诊断）。

### 函数签名

```cpp
[[nodiscard]] std::size_t pending_changes() const;
```

### 返回值

| 类型 | 说明 |
|------|------|
| `std::size_t` | 待通知的变更数量 |

### 使用示例

```cpp
using namespace cf::config;

// 检查是否有待处理变更
std::size_t count = ConfigStore::instance().pending_changes();

if (count > 0) {
    std::cout << "There are " << count << " pending changes" << std::endl;
    ConfigStore::instance().notify();
}

// 调试输出
void debug_config_state() {
    std::cout << "Pending changes: " << ConfigStore::instance().pending_changes() << std::endl;
}

// 确保所有变更已通知
void ensure_notified() {
    while (ConfigStore::instance().pending_changes() > 0) {
        ConfigStore::instance().notify();
    }
}
```

---

## 监听器使用模式

### 模式1: 实时响应（Immediate）

```cpp
// 配置变更立即生效
ConfigStore::instance().watch(
    "app.volume",
    [](const Key& k, const std::any* old, const std::any* neu, Layer) {
        if (neu) {
            int volume = std::any_cast<int>(*neu);
            set_system_volume(volume);
        }
    },
    NotifyPolicy::Immediate
);

// 设置音量时立即生效
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "volume"},
    80,
    Layer::App,
    NotifyPolicy::Immediate  // 立即触发回调
);
```

### 模式2: 批量处理（Manual）

```cpp
// 多项配置统一更新
ConfigStore::instance().watch(
    "app.ui.*",
    [](const Key& k, const std::any* old, const std::any* neu, Layer) {
        // 仅在 notify() 时调用一次
        refresh_ui();
    },
    NotifyPolicy::Manual
);

// 批量修改
ConfigStore::instance().set(
    KeyView{.group = "app.ui", .key = "theme"},
    "dark", Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app.ui", .key = "font"},
    "Arial", Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().set(
    KeyView{.group = "app.ui", .key = "size"},
    14, Layer::App, NotifyPolicy::Manual
);
ConfigStore::instance().notify();  // 统一触发
```

### 模式3: 条件监听

```cpp
// 根据条件启用/禁用监听
class ConditionalWatcher {
  public:
    void enable() {
        if (!handle_) {
            handle_ = ConfigStore::instance().watch("app.*", callback_);
        }
    }
    void disable() {
        if (handle_) {
            ConfigStore::instance().unwatch(*handle_);
            handle_ = std::nullopt;
        }
    }
  private:
    std::optional<WatcherHandle> handle_;
    Watcher callback_ = [](const Key&, const std::any*, const std::any*, Layer) {
        // 处理变更
    };
};
```

---

## 下一章

- [06-api-persist.md](./06-api-persist.md) - 持久化操作 API
