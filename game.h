/* Define the game object
 */

#ifndef GAME_H
#define GAME_H

#include "view.h"
#include "model.h"

#include <QObject>
#include <QTranslator>

/* Game Object
 * Own view and model object.
 * initialize, connect, call for load and show
 */
class Game : public QObject
{
    Q_OBJECT
public:
    Game() {
        view = new View();
        model = new Model();
        translator = new QTranslator();
        QApplication::instance()->installTranslator(translator);

        model->onLoadFile(":/resources/levels/七步成诗(7).klotski");
    }

    /* Connect model and view */
    void connect() {
        QObject::connect(model, &Model::syncMove, view,  &View::applyMove);
        QObject::connect(view,  &View::syncMove, model, &Model::applyMove);

        QObject::connect(model, &Model::piecesChanged,              view, &View::updatePieces);
        QObject::connect(model, &Model::validMovesChanged,          view, &View::updateValidMoves);
        QObject::connect(model, &Model::canWinStateChanged,         view, &View::updateCanWinStateChanged);
        QObject::connect(model, &Model::stepCountChanged,           view, &View::updateStepCount);
        QObject::connect(model, &Model::bestStepCountChanged,       view, &View::updateBestStepCount);
        QObject::connect(model, &Model::canUndoStateChanged,        view, &View::updateCanUndoStateChanged);
        QObject::connect(model, &Model::canRedoStateChanged,        view, &View::updateCanRedoStateChanged);
        QObject::connect(model, &Model::historyModelChanged,        view, &View::updateHistoryModel);
        QObject::connect(model, &Model::currentMoveIndexChanged,    view, &View::updateCurrentMoveIndex);
        QObject::connect(model, &Model::savedToFile,                view, &View::onSavedToFile);
        QObject::connect(model, &Model::levelNameChanged,           view, &View::updateLevelName);
        QObject::connect(model, &Model::fileNameChanged,            view, &View::updateFileName);

        QObject::connect(view, &View::requireModelDataRefresh,  model, &Model::onViewRequireDataRefresh);
        QObject::connect(view, &View::undo,                     model, &Model::onUndo);
        QObject::connect(view, &View::redo,                     model, &Model::onRedo);
        QObject::connect(view, &View::userSelectedHistory,      model, &Model::onUserSelectedHistory);
        QObject::connect(view, &View::reset,                    model, &Model::onReset);
        QObject::connect(view, &View::loadFile,                 model, &Model::onLoadFile);
        QObject::connect(view, &View::saveToFile,               model, &Model::onSaveToFile);

        QObject::connect(view, &View::changeTranslateToChineseSimplified,
                         this, &Game::onChangeTranslateToChineseSimplified);
        QObject::connect(view, &View::changeTranslateToEnglish,
                         this, &Game::onChangeTranslateToEnglish);
    }
    /* Start the game */
    void start() {
        connect();
        view->refresh();
        view->forceResize();
        view->show();
    }

private slots:
    /* About translate  */
    void onChangeTranslateToEnglish() {
        translator->load("");
        reloadView();
    }
    void onChangeTranslateToChineseSimplified() {
        translator->load(":/resources/translate/zh_CN.qm");
        reloadView();
    }

private:
    void reloadView() {
        view->close();
        view->deleteLater();

        view = new View;
        start();
    }

    View *view;
    Model *model;
    QTranslator *translator;
};

#endif // GAME_H
