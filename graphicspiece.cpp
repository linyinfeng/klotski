#include "graphicspiece.h"

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>

GraphicsPiece::GraphicsPiece(
    QGraphicsItem * parent,
    const QRect &rect,
    float scale,
    const QColor &color
) : QGraphicsObject(parent),
    m_scale(scale),
    m_color(color),
    m_piece(rect)
{
    initialize();
}

GraphicsPiece::GraphicsPiece(
    QGraphicsItem * parent,
    const QSize &size,
    float scale,
    const QColor &color
): QGraphicsObject(parent),
    m_scale(scale),
    m_color(color),
    m_piece(size)

{
    initialize();
}

void GraphicsPiece::initialize() {
    mouse_pressing = false;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
}

QRectF GraphicsPiece::graphicsRect() const {
    return QRectF(QPointF(m_piece.position()) * m_scale, QSizeF(m_piece.size()) * m_scale);
}

QRectF GraphicsPiece::boundingRect() const {
    return graphicsRect();
}

void GraphicsPiece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

void GraphicsPiece::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverEnterEvent(event);
    setFocus(Qt::MouseFocusReason);
    qDebug() << this << "hoverEnterEvent";
}
void GraphicsPiece::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverLeaveEvent(event);
    clearFocus();
    qDebug() << this << "mousePressEvent";
}
void GraphicsPiece::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverMoveEvent(event);
    qDebug() << this << "hoverMoveEvent";

}
void GraphicsPiece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mousePressEvent(event);
    qDebug() << this << "mousePressEvent" << event->button();
}
void GraphicsPiece::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseMoveEvent(event);
    qDebug() << this << "mouseMoveEvent" << event->button();
}
void GraphicsPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseReleaseEvent(event);
    qDebug() << this << "mouseReleaseEvent" << event->button();
}
void GraphicsPiece::keyPressEvent(QKeyEvent *event) {
    QGraphicsObject::keyPressEvent(event);
    qDebug() << this << "keyPressEvent";
}

