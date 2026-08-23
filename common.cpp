#include "common.h"
#include <QDir>
#include <QPoint>
#include <QStandardPaths>
#include <QString>
#include <QApplication>

extern const int kHorizontalUnit = 4;
extern const int kVerticalUnit = 5;

extern const int kWinPieceIndex = 0;
extern const QPoint kWinPosition(1, 3);

extern const QString kSaveSuffix("klotski");

extern const QString kDefaultLevelDir = "/levels";
extern const QString kDefaultSolutionDir = "/solutions";

extern const QString kViewSettingsFileName = "/view_settings.txt";
extern const QString kLanguageSettingsFileName = "/language_settings.txt";

extern const QString kAutoSaveFileName = "/auto_save.klotski";

QString dataDir()
{
    QStringList candidates = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    // ad-hoc nix builds (no profile/XDG_DATA_DIRS link) only see the store prefix
    candidates << QCoreApplication::applicationDirPath() + "/../share/klotski";
    for (const QString &dir : candidates) {
        if (QDir(dir + kDefaultLevelDir).exists())
            return dir;
    }
    return QCoreApplication::applicationDirPath();
}

QString writableDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir;
}
