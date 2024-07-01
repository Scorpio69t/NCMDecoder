#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    QFile file(":/skin/skin/darkblue.css");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
    }

    w.show();
    return a.exec();
}
