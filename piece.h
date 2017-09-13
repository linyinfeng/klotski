#ifndef PIECE_H
#define PIECE_H

#include <QObject>

class Piece : public QObject, public QRect
{
    Q_OBJECT
public:
    explicit Piece(QObject *parent = nullptr);
};

#endif // PIECE_H
