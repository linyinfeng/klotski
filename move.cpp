#include "move.h"

#include <cstddef>

QTextStream &operator << (QTextStream &stream, const Move &move) {
    stream << move.index() << "\t" << move.x() << "\t"  << move.y();
    return stream;
}
QTextStream &operator >> (QTextStream &stream, Move &move) {
    int index, x, y;
    stream >> index >> x >> y;
    move.index_ = index;
    move.setX(x);
    move.setY(y);
    return stream;
}

Move::Move(int index, int x, int y)
    : index_(index) {
    setX(x);
    setY(y);
    setId();
}

Move::Move() : Move(-1, 0, 0)  { }

Move::Move(const Move &move) : Move(move.index_, move.x_, move.y_) { }

Move &Move::operator= (const Move &move) {
    index_ = move.index_;
    x_ = move.x_;
    y_ = move.y_;
    return *this;
}

Direction Move::direction() {
    if (x_ > 0) {
        return Direction::right;
    } else if (x_ < 0) {
        return Direction::left;
    } else if (y_ < 0) {
        return Direction::up;
    } else if (y_ > 0) {
        return Direction::down;
    } else {
        return Direction::invalid;
    }
}

bool Move::operator == (const Move &move) const {
    return index_ == move.index_ && x_ == move.x_ && y_ == move.y_;
}

int Move::index() const { return index_; }
int Move::x()     const { return x_;     }
int Move::y()     const { return y_;     }
std::size_t Move::id() const { return id_; }

Move Move::reverse () const {
    return Move(index_, -x_, -y_);
}

bool Move::isNull() const {
    return !(x_ + y_);
}

void Move::setX(int x) { x_ = forceConvert(x); }
void Move::setY(int y) { y_ = forceConvert(y); }
void Move::setId() {
    static std::size_t id = 0;
    id_ = ++id; // start from 1
}

int Move::forceConvert(int n) {
    int res;
    if (n > 0)
        res = 1;
    else if (n == 0)
        res = 0;
    else
        res = -1;
    return res;
}

