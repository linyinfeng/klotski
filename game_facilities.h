#ifndef GAME_FACILITES_H
#define GAME_FACILITES_H

#include <QRect>
#include <QSize>
#include <QPoint>

namespace game {

class Move
{
public:
    Move(int index, int x, int y)
        : m_index(index), m_x(x), m_y(y) { }

    int index() const { return m_index; }
    int x()     const { return m_x;     }
    int y()     const { return m_y;     }

    Move reverse () {
        return Move(m_index, -m_x, -m_y);
    }

    void setIndex(int index) { m_index = index; }
    void setX(int x) { m_x = forceConvert(x); }
    void setY(int y) { m_x = forceConvert(y); }

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

    int m_index;
    int m_x;
    int m_y;
};

class Piece
{
public:
    Piece(const QRect &rect) : m_geometry(rect) { }
    explicit Piece(const QSize &size) : m_geometry(QPoint(0, 0), size) { }

    Piece(const Piece &piece) : m_geometry(piece.geometry()) { }

    QRect  geometry()  const { return m_geometry;           }
    QPoint position()  const { return m_geometry.topLeft(); }
    QSize  size()      const { return m_geometry.size();    }

    virtual void moveTo(const QPoint &pos) { m_geometry.moveTo(pos); }

    Piece &operator << (const Move &move) {
        QPoint pos(position().x() + move.x(), position().y() + move.y());
        moveTo(pos);
        return *this;
    }

private:
    QRect m_geometry;
};

}

#endif // GAME_FACILITIES
