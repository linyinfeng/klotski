#ifndef PIECE_H
#define PIECE_H

#include "move.h"
#include <QRect>

class Piece
{
public:
    Piece(const QRect &rect) : m_geometry(rect) { }
    explicit Piece(const QSize &size) : m_geometry(QPoint(0, 0), size) { }

    Piece(const Piece &piece) : m_geometry(piece.geometry()) { }

    QRect  geometry()  const { return m_geometry;           }
    QPoint position()  const { return m_geometry.topLeft(); }
    QSize  size()      const { return m_geometry.size();    }

    void moveTo(const QPoint &pos) { m_geometry.moveTo(pos); }

    Piece &operator << (const Move &move) {
        QPoint pos(position().x() + move.x(), position().y() + move.y());
        moveTo(pos);
        return *this;
    }

private:
    QRect m_geometry;
};

#endif // PIECE_H
