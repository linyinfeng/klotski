#ifndef MOVE_H
#define MOVE_H

#include <cstddef>

class Move
{
public:
    Move(int index, int x, int y)
        : index_(index) {
        setX(x);
        setY(y);
        setId();
    }

    Move() : Move(-1, 0, 0)  { }

    Move(const Move &move) : Move(move.index_, move.x_, move.y_) {}
    Move &operator= (const Move &move) {
        index_ = move.index_;
        x_ = move.x_;
        y_ = move.y_;
        return *this;
    }

    bool operator == (const Move &move) const {
        return index_ == move.index_ && x_ == move.x_ && y_ == move.y_;
    }

    int index() const { return index_; }
    int x()     const { return x_;     }
    int y()     const { return y_;     }
    std::size_t id() const { return id_; }

    Move reverse () const {
        return Move(index_, -x_, -y_);
    }

    bool isNull() const {
        return !(x_ + y_);
    }

private:
    void setX(int x) { x_ = forceConvert(x); }
    void setY(int y) { y_ = forceConvert(y); }

    void setId() {
        static std::size_t id = 0;
        id_ = ++id; // start from 1
    }

private:
    int forceConvert(int n) {
        int res;
        if (n > 0)
            res = 1;
        else if (n == 0)
            res = 0;
        else
            res = -1;
        return res;
    }

    int index_;
    int x_;
    int y_;

    std::size_t id_;
};

#endif // MOVE_H
