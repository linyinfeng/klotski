#include "mainwindow.h"
#include "gamemodel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    GameModel game_model;

    MainWindow window(game_model);
    window.show();

    return app.exec();
}
