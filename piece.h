#ifndef PIECE_H
#define PIECE_H

#include <QRect>

class Piece
{
public:
    explicit Piece(const QRect &rect) : m_geometry(rect) { }
    explicit Piece(const QSize &size) : m_geometry(QPoint(0, 0), size) { }

    QRect geometry() const { return m_geometry; }
//    void setGeometry(const QRect &rect) { m_geometry = rect; }

    QPoint position() const { return m_geometry.topLeft(); }
    virtual void setPosition(const QPoint &pos) { m_geometry.moveTo(pos); }

    QSize size() const { return m_geometry.size(); }
//    void setSize(const QSize &size) { m_geometry.setSize(size); }

private:
    QRect m_geometry;
};

#endif // PIECE_H
