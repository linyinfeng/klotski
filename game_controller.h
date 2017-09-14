#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "game_model.h"
#include "mainwindow.h"
#include "matrix.h"

#include <vector>

#include <QObject>
#include <QDebug>
#include <deque>
#include <vector>

namespace game {

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);

    unsigned stepCount();
    const std::vector<Move> &validMoves();

signals:
    void modelChanged(const game::Model *model);

    void winStateChanged(bool can_win);
    void validMovesChanged(const std::vector<Move> &valid_moves);

    void pieceChanged();
    void stepCountChanged(unsigned step);

public slots:
    void performMove(const Move &move);
    // if move failed, nothing happend
    void undo();
    void redo();

    void showWindow() {
        if (window)
            window->show();
        else
            qDebug() << "Error MainWindow is nullptr";
    }

private:
    void clearHistory() {
        m_history.erase(m_current + 1, m_history.end());
        m_history.erase(m_history.begin(), m_current);
    }
//    bool checkValid();
    void loadModelData();
    void generateLayout();

    void debugOutputLayout();

private:
    Matrix<int> *layout;

    std::deque<Move> m_history;
    std::deque<Move>::iterator m_current;

    // Model
    Model *m_model;
    // View
    MainWindow *window;
};

}

#endif // GAME_CONTROLLER_H

