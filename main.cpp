#include "common.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    game::Controller controller;
    controller.showWindow();

    return app.exec();
}
