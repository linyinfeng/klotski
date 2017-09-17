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
    }

    /* Connect model and view */
    void connect() {
        QObject::connect(model, &Model::canWinStateChanged,  view, &View::onCanWinStateChanged);
        QObject::connect(model, &Model::stepCountChanged,    view, &View::updateStepCount);
        QObject::connect(model, &Model::canUndoStateChanged, view, &View::onCanUndoStateChanged);
        QObject::connect(model, &Model::canRedoStateChanged, view, &View::onCanRedoStateChanged);

        QObject::connect(model, &Model::modelLoaded, view, &View::onModelLoaded);

        QObject::connect(model, &Model::syncMove, view,  &View::applyMove);
        QObject::connect(view,  &View::syncMove, model, &Model::applyMove);

        QObject::connect(model, &Model::validMovesChanged,
                         view,  &View::onValidMovesChanged);

        QObject::connect(model, &Model::currentMoveIndexChanged, view, &View::updateCurrentMoveIndex);
        QObject::connect(view,  &View::userSelectedHistory, model, &Model::onUserSelectedHistory);

        QObject::connect(view, &View::undo, model, &Model::onUndo);
        QObject::connect(view, &View::redo, model, &Model::onRedo);
        QObject::connect(view, &View::reload, model, &Model::onReload);

        QObject::connect(view, &View::loadFile, model, &Model::onLoadFile);
        QObject::connect(view, SIGNAL(save(const QString &)), model, SLOT(onSave(const QString &)));
        QObject::connect(model, SIGNAL(modelSaved(bool)), view, SLOT(onModelSaved(bool)));

        QObject::connect(view, SIGNAL(viewReload()), this, SLOT(onViewReload()));
    }
    /* Start the game */
    void start() {
        connect();
//        view.forceResize(); // a workaround on Hi-DPI screen
        view->setModel(model);
        model->onLoadFile(":/resources/levels/七步成诗 - 7.klotski");
        view->forceResize();
        view->show();
    }

private slots:
    void onViewReload() {
        delete view;
        view = new View();
        connect();
        view->setModel(model);
        view->forceResize();
        view->show();
    }
    /* About translate  */
    void onChangeTranslateToEnglish() {
        translator->load("");
    }
    void onChangeTranslateToChineseSimplified() {
        translator->load(":/resources/translate/zh_CN.qm");
    }

private:
    View *view;
    Model *model;
    QTranslator *translator;
};

#endif // GAME_H
