#include "game.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

//    qDebug() << "Application run in" << QCoreApplication::applicationDirPath();
//    int fontId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + "/fonts/inziu-iosevka-SC-regular.ttf" );
//    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);

//    QFont font;
//    if (fontFamilies.size() > 0)
//    {
//        font.setFamily(fontFamilies.at(0));
//        QApplication::setFont(font);
//        qDebug() << "Font Loaded";
//    }
    QApplication::setFont(QFont("Microsoft YaHei"));

    Game game;
    game.start();

    return a.exec();
}
