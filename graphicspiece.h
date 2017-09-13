#ifndef GRAPHICSPIECE_H
#define GRAPHICSPIECE_H

#include <QGraphicsItem>
#include <piece.h>

class GraphicsPiece : public QGraphicsItem, public Piece
{
public:
    GraphicsPiece(QObject *parent = nullptr);

    GraphicsPiece(QObject *parent = nullptr, double scale, const QColor &color);

    double scale() { return m_scale; }
    void setScale(double scale) { m_scale = scale; }

    const QColor &color() { return m_color; }
    void setColor(const QColor &color) { m_color = color; }

    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QColor m_color;
    double m_scale;
};

#endif // GRAPHICSPIECE_H
