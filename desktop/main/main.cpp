#include "cflog.h"
#include "init/init.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    QApplication cf_desktop_app(argc, argv);

    qDebug() << "Qt Application is Boot OK, About to run Early Boot...";

    cf::desktop::init_session::RunEarlyInit();

    // release sources and given everything for the desktop
    // itself
    cf::desktop::init_session::ReleaseEarlyInit();

    cf::log::traceftag("Main", "Boot Finished, Ready to launch everything!");
    auto quit_code = cf_desktop_app.exec();

    /* After our quit, we may need to do some deinits */
    qDebug() << "Desktop Dismissed with code: " << quit_code << ", Goodbye!";
}
