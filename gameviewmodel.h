#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "gamecommon.h"

#include <QObject>
#include <deque>
#include <vector>


class GameViewModel : public QObject
{
    Q_OBJECT
public:
    GameViewModel(QObject *parent = nullptr);

    const Layout &layout();

    const std::vector<Piece *> &pieces() { return m_pieces; }
    void setPieces(const std::vector<Piece *> &pieces) { m_pieces = pieces; }

    unsigned stepCount();

    bool canMove(const Move &move);

signals:
    void winStateChanged(bool can_win);
    void passiveUpdated();
    void stepCountChanged(unsigned step);

    void layoutChanged(const Layout &new_layout);

public slots:
    void onMove(const Move &move);
    // if move failed, nothing happend
    void onUndo();
    void onRedo();

    void setLayout(const Layout &layout) {
        m_layout = layout;
        emit layoutChanged(m_layout);
    }

private:
    void clearHistory();
    void cutFutureHistory();

private:
    std::vector<Piece *> m_pieces;

    Layout m_layout;
    std::deque<Move> m_history;
    std::deque<Move>::iterator m_current;
    unsigned m_step_count;
};

#endif // GAMEMODEL_H
