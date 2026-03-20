#include "desktop_main_path_resolvers.h"
#include <QDir>

namespace {
QString make(const char* root, const char* components) {
    QDir rootDir(QString::fromUtf8(root));
    return rootDir.absoluteFilePath(QString::fromUtf8(components));
}
} // namespace

namespace cf::desktop::path {

DesktopMainPathProvider::DesktopMainPathProvider(const char* desktop_active_root)
    : root{desktop_active_root} {}

QString DesktopMainPathProvider::absolutePath(const PathType p) {
    const char* component = nullptr;
    switch (p) {
        case PathType::Home:
            component = PathComponents::Home;
            break;
        case PathType::Desktop:
            component = PathComponents::Desktop;
            break;
        case PathType::Documents:
            component = PathComponents::Documents;
            break;
        case PathType::Downloads:
            component = PathComponents::Downloads;
            break;
        case PathType::Music:
            component = PathComponents::Music;
            break;
        case PathType::Pictures:
            component = PathComponents::Pictures;
            break;
        case PathType::Videos:
            component = PathComponents::Videos;
            break;
        case PathType::Apps:
            component = PathComponents::Apps;
            break;
        case PathType::Runtime:
            component = PathComponents::Runtime;
            break;
    }
    return make(root.data(), component);
}

} // namespace cf::desktop::path
