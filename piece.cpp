#include "move.h"
#include "piece.h"
#include <QRect>

QTextStream &operator << (QTextStream &stream, const Piece &piece) {
    stream << piece.geometry().x() << "\t" << piece.geometry().y() << "\t" << piece.geometry().width() << "\t" << piece.geometry().height();
    return stream;
}
QTextStream &operator >> (QTextStream &stream, Piece &piece) {
    int x, y, width, height;
    stream >> x >> y >> width >> height;
    piece.geometry_.setX(x);
    piece.geometry_.setY(y);
    piece.geometry_.setWidth(width);
    piece.geometry_.setHeight(height);
    return stream;
}

Piece::Piece(const QRect &rect) : geometry_(rect) { }
Piece::Piece(const QSize &size) : geometry_(QPoint(0, 0), size) { }
Piece::Piece(const Piece &piece) : geometry_(piece.geometry()) { }

QRect  Piece::geometry()  const { return geometry_;           }
QPoint Piece::position()  const { return geometry_.topLeft(); }
QSize  Piece::size()      const { return geometry_.size();    }

void Piece::moveTo(const QPoint &pos) { geometry_.moveTo(pos); }

Piece &Piece::operator << (const Move &move) {
    QPoint pos(position().x() + move.x(), position().y() + move.y());
    moveTo(pos);
    return *this;
}

