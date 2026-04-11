/**
 * @file mock_path_provider.h
 * @brief Mock path provider for testing ConfigStore.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

#include "cfconfig/cfconfig_path_provider.h"
#include <QDir>
#include <QFile>
#include <QString>

namespace cf::config::test {

/**
 * @brief Mock path provider for unit tests.
 *
 * Uses temporary directory for all config layers.
 * Safe for parallel testing (each instance gets unique subdirectory).
 *
 * System layer file is auto-created for testing convenience.
 *
 * Usage:
 * @code
 *   auto provider = std::make_shared<MockPathProvider>();
 *   ConfigStoreImpl store(provider);
 * @endcode
 */
class MockPathProvider : public IConfigStorePathProvider {
  public:
    /**
     * @brief Construct with a custom temp directory.
     *
     * @param temp_dir Base temporary directory (default: "/tmp/cfconfig_test")
     */
    explicit MockPathProvider(const QString& temp_dir = "/tmp/cfconfig_test")
        : temp_dir_(temp_dir) {
        // Auto-create system.ini for testing (test dir is always writable)
        QString systemPath = temp_dir_ + "/system_test.ini";
        QDir().mkpath(QFileInfo(systemPath).absolutePath());
        QFile file(systemPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
        }
    }

    QString system_path() const override {
        // Use a separate system file in temp dir
        return temp_dir_ + "/system_test.ini";
    }

    QString user_dir() const override { return temp_dir_ + "/user"; }

    QString user_filename() const override { return "user_test.ini"; }

    QString app_dir() const override { return temp_dir_ + "/app"; }

    QString app_filename() const override { return "app_test.ini"; }

    bool is_layer_enabled(int layer_index) const override {
        // All layers enabled for testing
        (void)layer_index;
        return true;
    }

    /**
     * @brief Get the temp directory for cleanup.
     */
    const QString& temp_dir() const { return temp_dir_; }

  private:
    QString temp_dir_;
};

/**
 * @brief Mock path provider that disables specific layers.
 *
 * Useful for testing layer-specific behavior.
 */
class LayerControlMockProvider : public MockPathProvider {
  public:
    /**
     * @brief Construct with disabled layers.
     *
     * @param disabled_layers Bitmask of layers to disable
     *                        bit 0 = System, bit 1 = User, bit 2 = App
     */
    explicit LayerControlMockProvider(int disabled_layers = 0)
        : MockPathProvider("/tmp/cfconfig_layer_test"), disabled_layers_(disabled_layers) {}

    QString system_path() const override {
        return (disabled_layers_ & 0x1) ? QString() : MockPathProvider::system_path();
    }

    QString user_dir() const override {
        return (disabled_layers_ & 0x2) ? QString() : MockPathProvider::user_dir();
    }

    QString app_dir() const override {
        return (disabled_layers_ & 0x4) ? QString() : MockPathProvider::app_dir();
    }

    bool is_layer_enabled(int layer_index) const override {
        return !(disabled_layers_ & (1 << layer_index));
    }

  private:
    int disabled_layers_;
};

/**
 * @brief Mock path provider that returns JSON filenames.
 *
 * Used to test JSON backend integration through ConfigStore.
 */
class JsonMockPathProvider : public IConfigStorePathProvider {
  public:
    explicit JsonMockPathProvider(const QString& temp_dir = "/tmp/cfconfig_json_test")
        : temp_dir_(temp_dir) {
        // Auto-create system.json for testing
        QString systemPath = temp_dir_ + "/system_test.json";
        QDir().mkpath(QFileInfo(systemPath).absolutePath());
        QFile file(systemPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write("{}");
            file.close();
        }
    }

    QString system_path() const override { return temp_dir_ + "/system_test.json"; }
    QString user_dir() const override { return temp_dir_ + "/user"; }
    QString user_filename() const override { return "user_test.json"; }
    QString app_dir() const override { return temp_dir_ + "/app"; }
    QString app_filename() const override { return "app_test.json"; }
    bool is_layer_enabled(int layer_index) const override {
        (void)layer_index;
        return true;
    }

    const QString& temp_dir() const { return temp_dir_; }

  private:
    QString temp_dir_;
};

} // namespace cf::config::test
