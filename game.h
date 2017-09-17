/* Define the game object
 */

#ifndef GAME_H
#define GAME_H

#include "view.h"
#include "model.h"
#include "move.h"

#include <QObject>

/* Game Object
 * Own view and model object.
 * initialize, connect, call for load and show
 */
class Game
{
public:
    Game() { }

    /* Connect model and view */
    void connect() {
        QObject::connect(&model, SIGNAL(canWinStateChanged(bool)),  &view, SLOT(onCanWinStateChanged(bool)));
        QObject::connect(&model, SIGNAL(stepCountChanged(int)),    &view, SLOT(updateStepCount(int)));
        QObject::connect(&model, SIGNAL(canUndoStateChanged(bool)), &view, SLOT(onCanUndoStateChanged(bool)));
        QObject::connect(&model, SIGNAL(canRedoStateChanged(bool)), &view, SLOT(onCanRedoStateChanged(bool)));

        QObject::connect(&model, SIGNAL(modelLoaded(const Model*)), &view, SLOT(onModelLoaded(const Model*)));

        QObject::connect(&model, SIGNAL(syncMove(const Move &)), &view,  SLOT(applyMove(const Move &)));
        QObject::connect(&view,  SIGNAL(syncMove(const Move &)), &model, SLOT(applyMove(const Move &)));

        QObject::connect(&model, SIGNAL(validMovesChanged(const std::vector<Move> &)),
                         &view,  SLOT(onValidMovesChanged(const std::vector<Move> &)));

        QObject::connect(&model, SIGNAL(currentMoveIndexChanged(int)), &view, SLOT(updateCurrentMoveIndex(int)));
        QObject::connect(&view,  SIGNAL(userSelectedHistory(int)), &model, SLOT(onUserSelectedHistory(int)));

        QObject::connect(&view, SIGNAL(undo()), &model, SLOT(onUndo()));
        QObject::connect(&view, SIGNAL(redo()), &model, SLOT(onRedo()));
        QObject::connect(&view, SIGNAL(reload()), &model, SLOT(onReload()));

        QObject::connect(&view, SIGNAL(load(const QString &)), &model, SLOT(onLoad(const QString &)));
        QObject::connect(&view, SIGNAL(save(const QString &)), &model, SLOT(onSave(const QString &)));
        QObject::connect(&model, SIGNAL(modelSaved(bool)), &view, SLOT(onModelSaved(bool)));
    }
    /* Start the game */
    void start() {
        connect();
//        view.forceResize(); // a workaround on Hi-DPI screen
        view.setModel(&model);
        model.onLoad(":/resources/levels/QiBuChengShi.klotski");
        view.forceResize();
        view.show();
    }

private:
    View view;
    Model model;
};

#endif // GAME_H
