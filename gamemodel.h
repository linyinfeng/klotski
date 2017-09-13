#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "gamecommon.h"
#include <QObject>
#include <deque>

namespace game {
    class Move;
    class Layout;
    class Piece;

    class Move {};
    class Layout {};
    class Piece {};
}

class GameModel : public QObject
{
    Q_OBJECT
public:
    GameModel(QObject *parent);

    const game::Layout &layout();
    void setLayout(const game::Layout &layout);

    unsigned stepCount();

signals:
    void winStateChanged(bool can_win);
    void passiveUpdated();
    void stepCountChanged(unsigned step);

public slots:
    void onMove(const game::Move &move);
    void onUndo();
    void onRedo();

private:
    void clearHistory();

private:
    game::Layout m_layout;
    std::deque<game::Move> m_history;
    std::deque<game::Move>::iterator m_current;
    unsigned m_step_count;
};

#endif // GAMEMODEL_H
