#include "qt_backend.h"
#include <QGuiApplication>

namespace cf::desktop::backend {

#ifdef CFDESKTOP_OS_LINUX

LinuxQtBackend GetBackend() {
    QString platformName = QGuiApplication::platformName();
    if (platformName == "xcb" || platformName == "x11") {
        return LinuxQtBackend::X11;
    } else if (platformName == "wayland") {
        return LinuxQtBackend::Wayland;
    }
    return LinuxQtBackend::Unknown;
}

#endif

} // namespace cf::desktop::backend
