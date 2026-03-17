# ConfigStore API - 查询操作

本文档介绍 ConfigStore 的配置查询 API。

## 目录

- [query() - 按优先级查询](#query---按优先级查询)
- [query() - 查询指定层](#query---查询指定层)
- [has_key() - 检查键存在性](#has_key---检查键存在性)

---

## query() - 按优先级查询

从所有层中按优先级查询配置值。

### 函数签名

```cpp
template <typename Value>
[[nodiscard]] std::optional<Value> query(const KeyView key);

template <typename Value>
[[nodiscard]] Value query(const KeyView key, const Value& default_value);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `KeyView` | 配置键视图 |
| `default_value` | `const Value&` | 默认值（仅重载2） |

### 返回值

| 重载 | 返回类型 | 说明 |
|------|----------|------|
| 1 | `std::optional<Value>` | 找到返回值，未找到返回 `std::nullopt` |
| 2 | `Value` | 找到返回值，未找到返回 `default_value` |

### 查询顺序

```
查询流程:
   KeyView: "app.theme.name"
        ↓
   ┌─────────────────────────────┐
   │  查找顺序: Temp → App → User │
   │                             │
   │  [Temp] 命中? ──Yes──→ 返回 │
   │      │ No                    │
   │      v                       │
   │  [App] 命中? ──Yes──→ 返回  │
   │      │ No                    │
   │      v                       │
   │  [User] 命中? ──Yes──→ 返回 │
   │      │ No                    │
   │      v                       │
   │  [System] 命中? ──Yes──→ 返回│
   │      │ No                    │
   │      v                       │
   │   返回 nullopt/default       │
   └─────────────────────────────┘
```

### 使用示例

```cpp
using namespace cf::config;

// 方式1: 使用 optional 检查是否存在
std::optional<std::string> theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"}
);

if (theme.has_value()) {
    std::cout << "Theme: " << theme.value() << std::endl;
} else {
    std::cout << "Theme not configured" << std::endl;
}

// 方式2: 使用默认值
std::string theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"},
    "default"  // 未找到时返回此值
);

// 查询数值类型
int width = ConfigStore::instance().query<int>(
    KeyView{.group = "app.window", .key = "width"},
    1280
);

// 查询布尔类型
bool fullscreen = ConfigStore::instance().query<bool>(
    KeyView{.group = "app.window", .key = "fullscreen"},
    false
);
```

---

## query() - 查询指定层

从指定层直接查询配置值，不使用优先级合并。

### 函数签名

```cpp
template <typename Value>
[[nodiscard]] std::optional<Value> query(const KeyView key, Layer layer);

template <typename Value>
[[nodiscard]] Value query(const KeyView key, Layer layer, const Value& default_value);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `KeyView` | 配置键视图 |
| `layer` | `Layer` | 目标层级 |
| `default_value` | `const Value&` | 默认值（仅重载2） |

### 返回值

| 重载 | 返回类型 | 说明 |
|------|----------|------|
| 1 | `std::optional<Value>` | 找到返回值，未找到返回 `std::nullopt` |
| 2 | `Value` | 找到返回值，未找到返回 `default_value` |

### 查询逻辑

```
指定层查询:
   KeyView: "app.theme.name", Layer: User
        ↓
   ┌─────────────────────────────┐
   │  仅在 User 层查找            │
   │                             │
   │  [Temp] ── 跳过              │
   │  [App]  ── 跳过              │
   │  [User] ── 查找 ──→ 命中/未命中│
   │  [System] ── 跳过             │
   └─────────────────────────────┘
```

### 使用示例

```cpp
using namespace cf::config;

// 查询用户层配置
std::optional<std::string> user_theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"},
    Layer::User
);

// 查询系统默认配置
std::string system_lang = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "language"},
    Layer::System,
    "en-US"
);

// 检查临时层配置（测试场景）
std::optional<int> temp_value = ConfigStore::instance().query<int>(
    KeyView{.group = "test", .key = "timeout"},
    Layer::Temp
);

// 对比不同层的值
std::string app_value = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.setting", .key = "mode"},
    Layer::App,
    "app-default"
);

std::string user_value = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.setting", .key = "mode"},
    Layer::User,
    "user-default"
);

std::cout << "App: " << app_value << ", User: " << user_value << std::endl;
```

---

## has_key() - 检查键存在性

检查配置键是否存在。

### 函数签名

```cpp
[[nodiscard]] bool has_key(const KeyView key);

[[nodiscard]] bool has_key(const KeyView key, Layer layer);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `key` | `KeyView` | 配置键视图 |
| `layer` | `Layer` | 目标层级（仅重载2） |

### 返回值

| 类型 | 说明 |
|------|------|
| `bool` | 键存在返回 `true`，否则返回 `false` |

### 查询逻辑

```
has_key(key):
   按优先级搜索所有层: Temp → App → User → System
   任一层命中即返回 true

has_key(key, layer):
   仅在指定层查找
```

### 使用示例

```cpp
using namespace cf::config;

// 检查键是否存在（任意层）
bool has_theme = ConfigStore::instance().has_key(
    KeyView{.group = "app.theme", .key = "name"}
);

if (has_theme) {
    // 安全地获取值
    std::string theme = ConfigStore::instance().query<std::string>(
        KeyView{.group = "app.theme", .key = "name"},
        "default"
    );
}

// 检查特定层的键
bool user_has_theme = ConfigStore::instance().has_key(
    KeyView{.group = "app.theme", .key = "name"},
    Layer::User
);

if (user_has_theme) {
    std::cout << "User has customized theme" << std::endl;
} else {
    std::cout << "Using default theme" << std::endl;
}

// 条件配置初始化
if (!ConfigStore::instance().has_key(KeyView{.group = "app", .key = "initialized"})) {
    // 首次运行，初始化默认配置
    ConfigStore::instance().set(
        KeyView{.group = "app", .key = "initialized"},
        true,
        Layer::App
    );
    setup_default_config();
}
```

---

## 类型转换

`query()` 支持自动类型转换：

| 存储类型 | 查询类型 | 转换 |
|----------|----------|------|
| `std::string` | `int` | 字符串转数字 |
| `std::string` | `double` | 字符串转浮点 |
| `std::string` | `bool` | 字符串转布尔 |
| `int` | `std::string` | 数字转字符串 |
| `double` | `std::string` | 浮点转字符串 |
| `bool` | `std::string` | 布尔转 "true"/"false" |
| `QVariant` | 基本类型 | QVariant 转换 |

### 转换示例

```cpp
using namespace cf::config;

// 存储为字符串，读取为整数
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "width"},
    std::string("1920"),
    Layer::App
);

int width = ConfigStore::instance().query<int>(
    KeyView{.group = "app", .key = "width"},
    1280
);  // width = 1920

// 存储为整数，读取为字符串
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "count"},
    42,
    Layer::App
);

std::string count = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app", .key = "count"},
    "0"
);  // count = "42"
```

---

## 下一章

- [04-api-write.md](./04-api-write.md) - 写入操作 API
