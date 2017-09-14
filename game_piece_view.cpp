#include "common.h"

#include <QGraphicsItem>
#include <QPainter>

#include <QTimer>
#include <QDebug>
#include <QPropertyAnimation>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>

namespace game {

PieceView::PieceView(
    QGraphicsItem *parent,
    const Piece *piece,
    float scale,
    const QColor &color
) : QGraphicsObject(parent),
    m_scale(scale),
    m_color(color),
    m_piece(piece)
{
    initialize();
}

void PieceView::initialize() {
    mouse_pressing = false;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
}

QRectF PieceView::graphicsRect() const {
    return m_rect;
}

void PieceView::resetRect() {
    m_rect = QRectF(QPointF(m_piece->position()) * m_scale,
                    QSizeF(m_piece->size()) * m_scale);
}

QRectF PieceView::boundingRect() const {
    return graphicsRect();
}

void PieceView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(m_color);
    painter->drawRect(graphicsRect());
//    qDebug() << "Draw" << graphicsRect();
    if (mouse_pressing) {
        painter->setBrush(m_color);
        painter->drawRect(graphicsRect());
    }
}

void PieceView::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverEnterEvent(event);
    setFocus(Qt::MouseFocusReason);
    qDebug() << this << "hoverEnterEvent";
}
void PieceView::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverLeaveEvent(event);
    clearFocus();
    qDebug() << this << "mousePressEvent";
}
void PieceView::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverMoveEvent(event);
    qDebug() << this << "hoverMoveEvent";

}
void PieceView::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mousePressEvent(event);
    qDebug() << this << "mousePressEvent" << event->button();
}
void PieceView::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseMoveEvent(event);
    qDebug() << this << "mouseMoveEvent" << event->button();
}
void PieceView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseReleaseEvent(event);
    qDebug() << this << "mouseReleaseEvent" << event->button();
}
void PieceView::keyPressEvent(QKeyEvent *event) {
    QGraphicsObject::keyPressEvent(event);

    int left = 0, up = 0;
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:
        up = m_scale;
        break;
    case Qt::Key_S: case Qt::Key_Down:
        up = -m_scale;
        break;
    case Qt::Key_A: case Qt::Key_Left:
        left = -m_scale;
        break;
    case Qt::Key_D: case Qt::Key_Right:
        left = m_scale;
        break;
    }

    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(200);
    animation->setStartValue(pos());
    animation->setEndValue(QPoint(pos().x() + left, pos().y() + up));
    animation->start();
    qDebug() << this << "keyPressEvent";
}

}

