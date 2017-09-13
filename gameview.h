#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "gamecommon.h"
#include "gamemodel.h"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class GameView : public QWidget
{
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr);

    GameModel *model();
    void setGameModel(GameModel *model);

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    GameModel *m_model;

signals:

public slots:
    void onGameViewResized();
};

#endif // GAMEVIEW_H
