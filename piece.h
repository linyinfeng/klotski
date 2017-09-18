#ifndef PIECE_H
#define PIECE_H

#include "move.h"
#include <QRect>

class Piece;

QTextStream &operator << (QTextStream &stream, const Piece &move);
QTextStream &operator >> (QTextStream &stream, Piece &move);

class Piece
{
    friend QTextStream &operator >> (QTextStream &stream, Piece &piece);

public:
    Piece(const QRect &rect);
    explicit Piece(const QSize &size);

    Piece(const Piece &piece);

    QRect  geometry()  const;
    QPoint position()  const;
    QSize  size()      const;

    void moveTo(const QPoint &pos);

    Piece &operator << (const Move &move);

private:
    QRect geometry_;
};

#endif // PIECE_H
