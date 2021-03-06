/* Define common properties of the game
 * Include only when needed
 */

#ifndef COMMON_H
#define COMMON_H

#include <QPoint>

/* common properties of klotski game */
extern const int kHorizontalUnit;
extern const int kVerticalUnit;
extern const int kWinPieceIndex;
extern const QPoint kWinPosition;
extern const QString kSaveSuffix;

extern const QString kDefaultLevelDir;
extern const QString kDefaultSolutionDir;

extern const QString kViewSettingsFileName;
extern const QString kLanguageSettingsFileName;

extern const QString kAutoSaveFileName;

#endif
