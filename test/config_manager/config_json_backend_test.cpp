/**
 * @file config_json_backend_test.cpp
 * @brief Tests for JSON config backend and ConfigStore JSON integration.
 *
 * Test Coverage:
 * 1. JsonConfigBackend - value, setValue, contains, remove, clear, sync, reload
 * 2. Nested key traversal (dot-separated keys → nested JSON objects)
 * 3. ConfigStore integration with JSON path provider
 * 4. Factory detection of .json vs .ini extensions
 * 5. Edge cases: missing file, corrupt JSON, key collision
 *
 * @date 2026-04-11
 * @version 1.0
 */

#include "cfconfig.hpp"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "mock_path_provider.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <gtest/gtest.h>
#include <memory>
#include <string>

// Forward-declare the backend classes for direct testing
#include "impl/config_backend.h"
#include "impl/config_backend_factory.h"
#include "impl/json_backend.h"
#include "impl/qsettings_backend.h"

#define DISCARD_RESULT(expr) (void)(expr)

// =============================================================================
// JsonConfigBackend Unit Tests
// =============================================================================

class JsonBackendTest : public ::testing::Test {
  protected:
    QTemporaryDir temp_dir_;

    void SetUp() override { ASSERT_TRUE(temp_dir_.isValid()); }

    void TearDown() override {}

    QString jsonPath() const { return temp_dir_.path() + "/test.json"; }

    QString iniPath() const { return temp_dir_.path() + "/test.ini"; }

    /// Helper: write raw JSON to file
    void writeJson(const QString& path, const QByteArray& data) {
        QDir().mkpath(QFileInfo(path).absolutePath());
        QFile file(path);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        file.write(data);
        file.close();
    }

    /// Helper: read raw JSON from file
    QByteArray readJson(const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            return {};
        return file.readAll();
    }

    /// Helper: parse JSON file content
    QJsonObject readJsonObj(const QString& path) {
        QByteArray data = readJson(path);
        if (data.isEmpty())
            return {};
        QJsonDocument doc = QJsonDocument::fromJson(data);
        return doc.isObject() ? doc.object() : QJsonObject();
    }
};

// --- Basic setValue / value ---

TEST_F(JsonBackendTest, SetValueAndQueryBasicTypes) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("int_key", 42);
    backend.setValue("double_key", 3.14);
    backend.setValue("bool_key", true);
    backend.setValue("string_key", QString("hello"));

    EXPECT_EQ(backend.value("int_key").toInt(), 42);
    EXPECT_DOUBLE_EQ(backend.value("double_key").toDouble(), 3.14);
    EXPECT_EQ(backend.value("bool_key").toBool(), true);
    EXPECT_EQ(backend.value("string_key").toString(), QString("hello"));
}

TEST_F(JsonBackendTest, ValueReturnsInvalidForMissingKey) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    QVariant v = backend.value("nonexistent");
    EXPECT_FALSE(v.isValid());
}

TEST_F(JsonBackendTest, ContainsReturnsTrueForExistingKey) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("exists", 1);
    EXPECT_TRUE(backend.contains("exists"));
    EXPECT_FALSE(backend.contains("no_such_key"));
}

// --- Nested keys ---

TEST_F(JsonBackendTest, NestedKeysCreateIntermediateObjects) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("app.theme.name", QString("dark"));
    backend.setValue("app.theme.color", QString("#333"));
    backend.setValue("app.window.width", 1024);

    EXPECT_EQ(backend.value("app.theme.name").toString(), QString("dark"));
    EXPECT_EQ(backend.value("app.theme.color").toString(), QString("#333"));
    EXPECT_EQ(backend.value("app.window.width").toInt(), 1024);
}

TEST_F(JsonBackendTest, DeepNestedKey) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("a.b.c.d.e", 99);
    EXPECT_EQ(backend.value("a.b.c.d.e").toInt(), 99);
    EXPECT_TRUE(backend.contains("a.b.c.d.e"));
}

TEST_F(JsonBackendTest, NestedValueMissingIntermediate) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    // "x.y.z" never set, querying should return invalid
    EXPECT_FALSE(backend.value("x.y.z").isValid());
    EXPECT_FALSE(backend.contains("x.y.z"));
}

// --- Remove ---

TEST_F(JsonBackendTest, RemoveTopLevelKey) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("to_remove", 42);
    EXPECT_TRUE(backend.contains("to_remove"));

    backend.remove("to_remove");
    EXPECT_FALSE(backend.contains("to_remove"));
}

TEST_F(JsonBackendTest, RemoveNestedKey) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("app.theme.name", QString("dark"));
    backend.setValue("app.theme.color", QString("#333"));

    backend.remove("app.theme.name");

    EXPECT_FALSE(backend.contains("app.theme.name"));
    // Sibling should still exist
    EXPECT_TRUE(backend.contains("app.theme.color"));
}

TEST_F(JsonBackendTest, RemoveNonexistentKeyIsNoop) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    EXPECT_NO_THROW(backend.remove("no_such_key"));
}

// --- Clear ---

TEST_F(JsonBackendTest, ClearRemovesAllKeys) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("a", 1);
    backend.setValue("b.c", 2);
    backend.clear();

    EXPECT_FALSE(backend.contains("a"));
    EXPECT_FALSE(backend.contains("b.c"));
}

// --- Sync ---

TEST_F(JsonBackendTest, SyncWritesToFile) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("synced_key", 123);
    backend.sync();

    QJsonObject obj = readJsonObj(jsonPath());
    ASSERT_TRUE(obj.contains("synced_key"));
    EXPECT_EQ(obj.value("synced_key").toInt(), 123);
}

TEST_F(JsonBackendTest, SyncWritesNestedStructure) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("app.theme.name", QString("light"));
    backend.setValue("app.window.width", 800);
    backend.sync();

    QJsonObject obj = readJsonObj(jsonPath());
    ASSERT_TRUE(obj.contains("app"));
    QJsonObject app = obj.value("app").toObject();
    ASSERT_TRUE(app.contains("theme"));
    EXPECT_EQ(app.value("theme").toObject().value("name").toString(), QString("light"));
    EXPECT_EQ(app.value("window").toObject().value("width").toInt(), 800);
}

TEST_F(JsonBackendTest, SyncCreatesParentDirectory) {
    QString nestedPath = temp_dir_.path() + "/deep/nested/dir/test.json";
    cf::config::JsonConfigBackend backend{nestedPath};

    backend.setValue("key", QString("value"));
    EXPECT_NO_THROW(backend.sync());

    QJsonObject obj = readJsonObj(nestedPath);
    EXPECT_EQ(obj.value("key").toString(), QString("value"));
}

// --- Reload ---

TEST_F(JsonBackendTest, ReloadReadsFromDisk) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("old_key", 1);
    backend.sync();

    // Modify file externally
    QByteArray newData = R"({"external_key": 999})";
    writeJson(jsonPath(), newData);

    backend.reload();

    EXPECT_FALSE(backend.contains("old_key"));
    EXPECT_TRUE(backend.contains("external_key"));
    EXPECT_EQ(backend.value("external_key").toInt(), 999);
}

// --- Load from existing file ---

TEST_F(JsonBackendTest, LoadFromExistingFile) {
    QByteArray initial = R"({
        "existing": {
            "nested": {
                "value": 42
            }
        },
        "top": "hello"
    })";
    writeJson(jsonPath(), initial);

    cf::config::JsonConfigBackend backend{jsonPath()};

    EXPECT_EQ(backend.value("existing.nested.value").toInt(), 42);
    EXPECT_EQ(backend.value("top").toString(), QString("hello"));
}

// --- Edge cases ---

TEST_F(JsonBackendTest, EmptyFilePathCreatesEmptyBackend) {
    cf::config::JsonConfigBackend backend{QString()};

    EXPECT_FALSE(backend.contains("any_key"));
    EXPECT_NO_THROW(backend.sync()); // should not crash
}

TEST_F(JsonBackendTest, NonexistentFileCreatesEmptyBackend) {
    cf::config::JsonConfigBackend backend{temp_dir_.path() + "/nonexistent.json"};

    EXPECT_FALSE(backend.contains("any_key"));
}

TEST_F(JsonBackendTest, CorruptJsonFileCreatesEmptyBackend) {
    writeJson(jsonPath(), "this is not valid json {{{");

    cf::config::JsonConfigBackend backend{jsonPath()};

    EXPECT_FALSE(backend.contains("any_key"));
}

TEST_F(JsonBackendTest, KeyCollisionOverwritesScalarWithObject) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("conflict", 42);
    EXPECT_EQ(backend.value("conflict").toInt(), 42);

    // Now set a deeper key under the same name
    backend.setValue("conflict.nested", QString("replaced"));
    EXPECT_EQ(backend.value("conflict.nested").toString(), QString("replaced"));
}

TEST_F(JsonBackendTest, OverwriteExistingValue) {
    cf::config::JsonConfigBackend backend{jsonPath()};

    backend.setValue("key", 1);
    EXPECT_EQ(backend.value("key").toInt(), 1);

    backend.setValue("key", 2);
    EXPECT_EQ(backend.value("key").toInt(), 2);
}

// =============================================================================
// Factory Tests
// =============================================================================

TEST(FactoryTest, JsonExtensionCreatesJsonBackend) {
    auto backend = cf::config::createBackend("/tmp/test.json");
    ASSERT_NE(backend, nullptr);
    EXPECT_NE(dynamic_cast<cf::config::JsonConfigBackend*>(backend.get()), nullptr);
}

TEST(FactoryTest, JsonExtensionCaseInsensitive) {
    auto backend = cf::config::createBackend("/tmp/test.JSON");
    ASSERT_NE(backend, nullptr);
    EXPECT_NE(dynamic_cast<cf::config::JsonConfigBackend*>(backend.get()), nullptr);
}

TEST(FactoryTest, IniExtensionCreatesQSettingsBackend) {
    auto backend = cf::config::createBackend("/tmp/test.ini");
    ASSERT_NE(backend, nullptr);
    EXPECT_NE(dynamic_cast<cf::config::QSettingsBackend*>(backend.get()), nullptr);
}

TEST(FactoryTest, EmptyPathReturnsNullptr) {
    auto backend = cf::config::createBackend(QString());
    EXPECT_EQ(backend, nullptr);
}

TEST(FactoryTest, UnknownExtensionDefaultsToIni) {
    auto backend = cf::config::createBackend("/tmp/test.conf");
    ASSERT_NE(backend, nullptr);
    EXPECT_NE(dynamic_cast<cf::config::QSettingsBackend*>(backend.get()), nullptr);
}

// =============================================================================
// ConfigStore JSON Integration Tests
// =============================================================================

class ConfigStoreJsonTest : public ::testing::Test {
  protected:
    QTemporaryDir temp_dir_;
    static bool json_initialized_;
    static QString json_test_path_;

    void SetUp() override {
        if (!json_initialized_) {
            ASSERT_TRUE(temp_dir_.isValid());
            json_test_path_ = temp_dir_.path() + "/cfconfig_json_test";
            auto json_provider =
                std::make_shared<cf::config::test::JsonMockPathProvider>(json_test_path_);
            cf::config::ConfigStore::instance().initialize(json_provider);
            json_initialized_ = true;
        }
        cf::config::ConfigStore::instance().clear();
    }

    void TearDown() override {
        cf::config::ConfigStore::instance().clear_layer(cf::config::Layer::Temp);
    }
};

bool ConfigStoreJsonTest::json_initialized_ = false;
QString ConfigStoreJsonTest::json_test_path_;

TEST_F(ConfigStoreJsonTest, SetAndQueryWithJsonBackend) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView kv{.group = "app", .key = "theme"};
    EXPECT_TRUE(store.set(kv, std::string("dark"), Layer::App));
    auto result = store.query<std::string>(kv);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "dark");
}

TEST_F(ConfigStoreJsonTest, NestedKeysWithJsonBackend) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView host_kv{.group = "network.proxy", .key = "host"};
    KeyView port_kv{.group = "network.proxy", .key = "port"};

    EXPECT_TRUE(store.set(host_kv, std::string("localhost"), Layer::User));
    EXPECT_TRUE(store.set(port_kv, 8080, Layer::User));

    auto host = store.query<std::string>(host_kv);
    auto port = store.query<int>(port_kv);

    ASSERT_TRUE(host.has_value());
    EXPECT_EQ(host.value(), "localhost");
    ASSERT_TRUE(port.has_value());
    EXPECT_EQ(port.value(), 8080);
}

TEST_F(ConfigStoreJsonTest, LayerPriorityWithJsonBackend) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView kv{.group = "test", .key = "priority"};

    DISCARD_RESULT(store.set(kv, 1, Layer::System));
    DISCARD_RESULT(store.set(kv, 2, Layer::User));
    DISCARD_RESULT(store.set(kv, 3, Layer::App));

    // App > User > System
    auto result = store.query<int>(kv);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 3);
}

TEST_F(ConfigStoreJsonTest, SyncPersistsToDisk) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView kv{.group = "test", .key = "persist"};
    DISCARD_RESULT(store.set(kv, std::string("value"), Layer::App));
    store.sync();

    // Verify the JSON file on disk contains the expected structure
    QString appFile = json_test_path_ + "/app/app_test.json";
    QFile file(appFile);
    ASSERT_TRUE(file.exists());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    ASSERT_TRUE(doc.isObject());
    QJsonObject root = doc.object();
    ASSERT_TRUE(root.contains("test"));
    QJsonObject testObj = root.value("test").toObject();
    EXPECT_EQ(testObj.value("persist").toString(), QString("value"));
}

TEST_F(ConfigStoreJsonTest, BoolAndDoubleTypes) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView bool_kv{.group = "types", .key = "flag"};
    KeyView dbl_kv{.group = "types", .key = "ratio"};

    DISCARD_RESULT(store.set(bool_kv, true, Layer::App));
    DISCARD_RESULT(store.set(dbl_kv, 2.718, Layer::User));

    auto b = store.query<bool>(bool_kv);
    auto d = store.query<double>(dbl_kv);

    ASSERT_TRUE(b.has_value());
    EXPECT_TRUE(b.value());
    ASSERT_TRUE(d.has_value());
    EXPECT_DOUBLE_EQ(d.value(), 2.718);
}

TEST_F(ConfigStoreJsonTest, HasKeyWithJsonBackend) {
    using namespace cf::config;
    auto& store = ConfigStore::instance();

    KeyView kv{.group = "test", .key = "exists"};
    EXPECT_FALSE(store.has_key(kv));
    DISCARD_RESULT(store.set(kv, 42, Layer::App));
    EXPECT_TRUE(store.has_key(kv));
}
