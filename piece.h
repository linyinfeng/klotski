#ifndef PIECE_H
#define PIECE_H

#include "move.h"
#include <QRect>

class Piece
{
public:
    Piece(const QRect &rect) : geometry_(rect) { }
    explicit Piece(const QSize &size) : geometry_(QPoint(0, 0), size) { }

    Piece(const Piece &piece) : geometry_(piece.geometry()) { }

    QRect  geometry()  const { return geometry_;           }
    QPoint position()  const { return geometry_.topLeft(); }
    QSize  size()      const { return geometry_.size();    }

    void moveTo(const QPoint &pos) { geometry_.moveTo(pos); }

    Piece &operator << (const Move &move) {
        QPoint pos(position().x() + move.x(), position().y() + move.y());
        moveTo(pos);
        return *this;
    }

private:
    QRect geometry_;
//    QString name_;
};

#endif // PIECE_H
