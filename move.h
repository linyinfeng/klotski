#ifndef MOVE_H
#define MOVE_H

#include <cstddef>
#include <QMetaType>
#include <QTextStream>

class Move;

QTextStream &operator << (QTextStream &stream, const Move &move);
QTextStream &operator >> (QTextStream &stream, Move &move);

enum class Direction {
    up, down, left, right, invalid
};

class Move
{
    friend QTextStream &operator >> (QTextStream &stream, Move &move);

public:
    Move(int index, int x, int y);
    Move();
    Move(const Move &move);
    Move &operator= (const Move &move);
    Direction direction();
    bool operator == (const Move &move) const;
    int index() const;
    int x()     const;
    int y()     const;
    std::size_t id() const;
    Move reverse () const;
    bool isNull() const;

private:
    void setX(int x);
    void setY(int y);

    void setId();

private:
    int forceConvert(int n);

    int index_;
    int x_;
    int y_;

    std::size_t id_;
};

Q_DECLARE_METATYPE(const Move *)
Q_DECLARE_METATYPE(Move *)

#endif // MOVE_H
