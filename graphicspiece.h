#ifndef GRAPHICSPIECE_H
#define GRAPHICSPIECE_H

#include <QGraphicsItem>
#include "gamecommon.h"
#include "piece.h"

class GraphicsPiece : public QGraphicsObject, public Piece
{
    Q_OBJECT
public:
    GraphicsPiece(QGraphicsItem * parent, const QRect &rect, double scale, const QColor &color = Qt::black);
    GraphicsPiece(QGraphicsItem * parent, const QSize &size, double scale, const QColor &color = Qt::black);
    void initializeState();

    double scale() const { return m_scale; }

    const QColor &color() const { return m_color; }

    QRectF graphicsRect() const;

    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

public slots:
    void setScale(double scale) { m_scale = scale; }
    void setColor(const QColor &color) { m_color = color; }

private:
    double m_scale;
    QColor m_color;

    // States
    bool mouse_pressing;
};

#endif // GRAPHICSPIECE_H
