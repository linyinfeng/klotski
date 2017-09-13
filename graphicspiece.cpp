#include "graphicspiece.h"
#include <QGraphicsItem>

GraphicsPiece::GraphicsPiece(QObject *parent = nullptr)
    : QObject(parent), Piece(0, 0, 1, 1), color(Qt::red)
{
}

QRectF GraphicsPiece::boundingRect()
{
    return m_scale *
}
