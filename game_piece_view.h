#ifndef GAME_PIECE_VIEW_H
#define GAME_PIECE_VIEW_H

#include "game_model.h"

#include <QGraphicsItem>

namespace game {

class PieceView : public QGraphicsObject
{
    Q_OBJECT
public:
    // Init
    PieceView(const Piece *piece,
              float scale,
              const QColor &color = Qt::black);
    void initialize();

public:
    QRectF graphicsRect() const;
    virtual QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public:
    float scale() const;
    QColor color() const;
    const Piece *piece() const;

signals:
    // Notify
    void scaleChanged(float scale);

public slots:
    void setScale(float scale) {
        m_scale = scale;
    }
    void setColor(const QColor &color) {
        m_color = color;
    }

private:
    void resetRect();

    float m_scale;
    QColor m_color;

    QRectF m_rect;

    const Piece *m_piece;

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

}

#endif // GAME_PIECE_VIEW_H
