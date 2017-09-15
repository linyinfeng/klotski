#ifndef GRAPHICSPIECE_H
#define GRAPHICSPIECE_H

#include "piece.h"
#include "move.h"
#include <QGraphicsObject>
#include <QRectF>
#include <QPointF>
#include <QObject>

class GraphicsPiece : public QGraphicsObject
{
    Q_OBJECT
public:
    GraphicsPiece(int index, const Piece &piece);
public:
    void addValidMoveDirection(const Move &valid_move);
    void clearValidMoveDirection();
signals:
    void syncMove(const Move &move);
public slots:
    void onSyncMove(const Move &move);
    void onSceneResize();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    virtual QRectF boundingRect() const override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
//    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    QRectF calcRect(const Piece &piece);
    QPointF calcPosition(const Piece &piece);
    QPointF calcPosition(const QPoint &point);

    void applyMove(const Move &move);

    bool can_move_up;
    bool can_move_down;
    bool can_move_left;
    bool can_move_right;

    bool hovered_;
    bool pressed_;

    qreal scale_;
    QRectF rect_; // topLeft vertex is always (0, 0)

    Piece piece_;
    int index_;
};

#endif // GRAPHICSPIECE_H
