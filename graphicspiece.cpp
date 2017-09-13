#include "graphicspiece.h"

#include <QGraphicsItem>
#include <QPainter>

GraphicsPiece::GraphicsPiece(QGraphicsItem * parent, const QRect &rect, double scale, const QColor &color)
    : QGraphicsObject(parent), Piece(rect), m_scale(scale), m_color(color) { initializeState(); }
GraphicsPiece::GraphicsPiece(QGraphicsItem * parent, const QSize &size, double scale, const QColor &color)
    : QGraphicsObject(parent), Piece(size), m_scale(scale), m_color(color) { initializeState(); }

void GraphicsPiece::initializeState() {
    mouse_pressing = false;
}

QRectF GraphicsPiece::graphicsRect() const {
    return QRectF(Piece::position() * m_scale, Piece::size() * m_scale);
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
    if (mouse_pressing) {
        painter->setBrush(m_color);
        painter->drawRect(graphicsRect());
    }
}

void GraphicsPiece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mousePressEvent(event);
    mouse_pressing = true;
}
void GraphicsPiece::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseMoveEvent(event);
}
void GraphicsPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseReleaseEvent(event);
    mouse_pressing = true;
}
void GraphicsPiece::keyPressEvent(QKeyEvent *event) {
    QGraphicsObject::keyPressEvent(event);
}

