# ConfigStore API - 持久化操作

本文档介绍 ConfigStore 的配置持久化 API。

## 目录

- [sync() - 同步到磁盘](#sync---同步到磁盘)
- [reload() - 从磁盘重载](#reload---从磁盘重载)

---

## sync() - 同步到磁盘

将配置变更写入磁盘持久化存储。

### 函数签名

```cpp
void sync(const SyncMethod m = SyncMethod::Async);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `m` | `SyncMethod` | 同步方式，默认 `Async` |

### SyncMethod 枚举

```cpp
enum class SyncMethod { Sync, Async };
```

| 值 | 说明 | 阻塞 |
|------|------|------|
| `Sync` | 同步写入，等待完成 | 是 |
| `Async` | 异步写入，立即返回 | 否 |

### 同步行为

```
sync() 执行流程:
   ┌─────────────────────────────┐
   │  检查各层是否有变更         │
   │  仅写入有变更的层           │
   │                             │
   │  [Temp] ── 跳过（不持久化） │
   │  [App]  ── 写入 app.conf   │
   │  [User] ── 写入 user.conf  │
   │  [System] ── 写入 sys.conf │
   └─────────────────────────────┘
        ↓
   Sync:   阻塞直到写入完成
   Async:  立即返回，后台写入
```

### 使用示例

```cpp
using namespace cf::config;

// 异步同步（默认）
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "theme"},
    std::string("dark"),
    Layer::App
);
ConfigStore::instance().sync();  // 默认 Async，立即返回

// 同步同步（确保写入完成）
std::string sensitive_value = "critical_data_value";
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "critical_data"},
    sensitive_value,
    Layer::User
);
ConfigStore::instance().sync(SyncMethod::Sync);  // 等待写入完成
std::cout << "Data safely stored" << std::endl;

// 关闭前同步
void on_app_shutdown() {
    ConfigStore::instance().sync(SyncMethod::Sync);  // 确保数据保存
}

// 定期自动同步
void periodic_sync() {
    ConfigStore::instance().sync(SyncMethod::Async);
}
```

### Sync vs Async

| 场景 | 推荐方式 | 原因 |
|------|----------|------|
| 普通配置变更 | `Async` | 不阻塞 UI，性能更好 |
| 关键数据保存 | `Sync` | 确保写入成功 |
| 应用退出 | `Sync` | 防止数据丢失 |
| 批量修改后 | `Async` 或 `Sync` | 根据重要性选择 |

---

## reload() - 从磁盘重载

从磁盘重新加载所有配置文件，清除内存缓存。

### 函数签名

```cpp
void reload();
```

### 重载行为

```
reload() 执行流程:
   ┌─────────────────────────────┐
   │  1. 清除内存缓存            │
   │  2. 丢弃 Temp 层所有数据    │
   │  3. 重新读取各层配置文件    │
   │                             │
   │  [Temp]  ── 清空            │
   │  [App]   ── 重新加载        │
   │  [User]  ── 重新加载        │
   │  [System] ── 重新加载       │
   └─────────────────────────────┘
```

### 影响范围

| 层级 | 重载行为 |
|------|----------|
| `Temp` | **完全清空**，所有临时配置丢失 |
| `App` | 从文件重新加载 |
| `User` | 从文件重新加载 |
| `System` | 从文件重新加载 |

### 使用示例

```cpp
using namespace cf::config;

// 基本使用 - 重载所有配置
ConfigStore::instance().reload();

// 场景1: 检测到外部配置文件修改
void on_config_file_changed(const std::string& path) {
    std::cout << "Config file modified: " << path << std::endl;
    ConfigStore::instance().reload();
    apply_config_to_ui();
}

// 场景2: 用户切换
void on_user_switch(User new_user) {
    // 切换用户配置文件路径
    update_config_path_for_user(new_user);
    // 重载配置
    ConfigStore::instance().reload();
}

// 场景3: 恢复默认设置
void restore_defaults() {
    // 删除用户配置文件
    delete_user_config_file();
    // 重载（将使用系统默认）
    ConfigStore::instance().reload();
}

// 场景4: 测试后清理
void cleanup_test_config() {
    // 清除测试期间设置的临时配置
    ConfigStore::instance().reload();
}
```

### 注意事项

- `reload()` 会**丢弃** `Temp` 层的所有数据
- 内存中未 `sync()` 的变更将丢失
- 重载后已注册的 `Watcher` 仍然有效

---

## 持久化存储路径

配置文件由 `IConfigStorePathProvider` 提供，默认路径如下：

```
默认存储路径:
   System: /etc/cfdesktop/config/system.conf
   User:   ~/.config/cfdesktop/user.conf
   App:    ~/.config/cfdesktop/app.conf
   Temp:   (不持久化，仅内存)
```

### 自定义路径

```cpp
// 使用自定义路径提供者初始化
auto custom_provider = std::make_shared<MyPathProvider>("/custom/path");
ConfigStore::instance().initialize(custom_provider);
```

---

## 持久化操作模式

### 模式1: 自动持久化

```cpp
// 每次修改后异步同步
void set_config_and_sync(const KeyView& key, const auto& value) {
    ConfigStore::instance().set(key, value, Layer::App);
    ConfigStore::instance().sync(SyncMethod::Async);
}
```

### 模式2: 延迟持久化

```cpp
// 收集变更，定期同步
std::chrono::steady_clock::time_point last_sync;

void set_config_lazy(const KeyView& key, const auto& value) {
    ConfigStore::instance().set(key, value, Layer::App);

    auto now = std::chrono::steady_clock::now();
    if (now - last_sync > std::chrono::seconds(30)) {
        ConfigStore::instance().sync(SyncMethod::Async);
        last_sync = now;
    }
}
```

### 模式3: 关键数据立即持久化

```cpp
// 关键配置使用同步写入
void save_critical_config(const KeyView& key, const auto& value) {
    ConfigStore::instance().set(key, value, Layer::User);
    ConfigStore::instance().sync(SyncMethod::Sync);  // 确保写入
}
```

### 模式4: 事务式保存

```cpp
// 假设 ConfigChange 是用户定义的包含 KeyView 和值的类型
// struct ConfigChange {
//     KeyView key;
//     std::any value;
// };

// 批量修改后统一同步
void update_config_batch(const std::vector<ConfigChange>& changes) {
    // 1. 备份当前配置
    backup_current_config();

    // 2. 批量修改
    for (const auto& change : changes) {
        ConfigStore::instance().set(
            change.key,
            change.value,
            Layer::App,
            NotifyPolicy::Manual
        );
    }

    // 3. 同步保存
    ConfigStore::instance().sync(SyncMethod::Sync);

    // 4. 触发通知
    ConfigStore::instance().notify();
}
```

---

## 完整生命周期示例

```cpp
using namespace cf::config;

// 应用启动
void app_startup() {
    // 1. 初始化（自动加载配置）
    ConfigStore::instance().initialize(custom_path_provider);

    // 2. 注册监听器
    ConfigStore::instance().watch("app.*", config_change_handler);

    // 3. 应用配置
    apply_config_to_ui();
}

// 应用运行
void app_runtime() {
    // 修改配置
    ConfigStore::instance().set(
        KeyView{.group = "app", .key = "theme"},
        std::string("dark"),
        Layer::App
    );

    // 异步同步
    ConfigStore::instance().sync(SyncMethod::Async);
}

// 应用关闭
void app_shutdown() {
    // 1. 同步保存（确保数据不丢失）
    ConfigStore::instance().sync(SyncMethod::Sync);

    // 2. 取消监听
    // (RAII 自动处理或手动 unwatch)
}
```

---

## 下一章

- [07-api-singleton.md](./07-api-singleton.md) - 单例访问 API
