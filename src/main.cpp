#include "ui/MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QLocale>
#include <windows.h>

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    QApplication app(argc, argv);

    qputenv("QT_LANG", "zh_CN");
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
    qputenv("QT_FONT_DPI", "96");
    
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));

    app.setStyle(QStyleFactory::create("Fusion"));

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(style);
    }

    MainWindow window;
    window.show();

    return app.exec();
}
