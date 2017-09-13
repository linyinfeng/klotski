#ifndef GRAPHICSPIECE_H
#define GRAPHICSPIECE_H

#include <QGraphicsItem>
#include "gamecommon.h"
#include "piece.h"

class GraphicsPiece : public QGraphicsObject
{
    Q_OBJECT
public:
    // Init
    GraphicsPiece(QGraphicsItem * parent, const QRect &rect, float scale, const QColor &color = Qt::black);
    GraphicsPiece(QGraphicsItem * parent, const QSize &size, float scale, const QColor &color = Qt::black);
    void initialize();

public:
    QRectF graphicsRect() const;
    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    // Properties
    Q_PROPERTY(float scale
               READ scale
               WRITE setScale
               NOTIFY scaleChanged)
    Q_PROPERTY(QColor color
               READ color
               WRITE setColor)

    // Read
    double scale() const { return m_scale; }
    const QColor &color() const { return m_color; }

signals:
    // Notify
    void scaleChanged(float scale);

public slots:
    // Set
    void setScale(float scale) { m_scale = scale; emit scaleChanged(scale); }
    void setColor(const QColor &color) { m_color = color; }

private:
    float m_scale;
    QColor m_color;

    Piece m_piece;

private:
    // Private data
    // States
    bool mouse_pressing;

protected:
    // Event
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // GRAPHICSPIECE_H
