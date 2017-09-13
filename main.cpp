#include "mainwindow.h"
#include "gameviewmodel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
