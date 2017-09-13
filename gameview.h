#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "gamecommon.h"
#include "gamemodel.h"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHoverEvent>
#include <QList>

#include "graphicspiece.h"

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr);

    Q_PROPERTY(float scale
               READ scale
               WRITE setScale
               NOTIFY scaleChanged)

    Q_PROPERTY(GameModel *model
               READ model
               WRITE setModel)
public:
    // Read
    GameModel *model() { return m_model; }
    float scale() { return m_scale; }
public slots:
    // Write
    void setModel(GameModel *model) { m_model = model; }
    void setScale(float scale) { m_scale = scale; emit scaleChanged(m_scale); }
signals:
    void scaleChanged(float new_scale);

private:
    // Member
    QGraphicsScene *m_scene;
    float m_scale;

    QList<GraphicsPiece *> pieces;
    void createNewPiece(const QRect &rect);

private:
    // linked model
    GameModel *m_model;

public slots:
    void onGameViewResized();

protected:
    virtual void enterEvent(QEvent *event) override;
};

#endif // GAMEVIEW_H
