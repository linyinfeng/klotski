//#define IGNORE_VALID_MOVES

#include "common.h"
#include "graphicspiece.h"
#include "move.h"

#include <QPainter>
#include <QWidget>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <cmath>

GraphicsPiece::GraphicsPiece(int index, const Piece &piece)
    : can_move_up(false), can_move_down(false),
      can_move_left(false), can_move_right(false),
      piece_(piece), index_(index)
{
    hovered_ = false;
    pressed_ = false;
    focused_ = false;
    qDebug() << "New GraphicsPiece" << index_ << piece_.geometry();
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
//    onSceneResize(); // set scale_ and rect_ and position_
}

void GraphicsPiece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen pen;

    pen.setColor(Qt::black);
    if (focused_) {
        pen.setWidth(3);
    } else {
        pen.setWidth(2);
    }
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    painter->setPen(pen);
    if (pressed_)
        painter->setBrush(QBrush(QColor(200, 200, 200)));
    else
        painter->setBrush(QBrush(Qt::gray));
    painter->drawRoundedRect(rect_, scale_ * 0.05, scale_ * 0.05);
}

QRectF GraphicsPiece::boundingRect() const {
    return QRectF(QPointF(0, 0), piece_.size() * scale_);
    // ignore free space for easy drawing
}

void GraphicsPiece::onSceneResize() {
    if (scene() != nullptr) {
        scale_ = scene()->sceneRect().width() / kHorizontalUnit;
        rect_ = calcRect(piece_);
        setPos(calcPosition(piece_));

        qDebug() << "GraphicsPiece" << index_ << "boundingRect" << boundingRect() << "pos" << pos();
        update();
    } else {
        qDebug() << "Invalid" << "scene_";
        rect_ = QRectF(0, 0, 0, 0);
    }
}

QRectF GraphicsPiece::calcRect(const Piece &piece) {
    qreal space_height = scale_ * piece.size().height();
    qreal space_width  = scale_ * piece.size().width();
    qreal free_space = scale_ * 0.05;
    QRectF res(0, 0, 0, 0);
    res.setSize(QSizeF(space_width - 2 * free_space, space_height - 2 * free_space));
    res.moveTopLeft(QPointF( free_space, free_space));
    qDebug() << "calcRect" << "piece" << piece.geometry() << "calculated" << res;
    return res;
}

QPointF GraphicsPiece::calcPosition(const Piece &piece) {
    QPointF pos(piece.position().x() * scale_, piece.position().y() * scale_);
    qDebug() << "calcPosition" << "piece" << piece.geometry() << "calculated" << pos;
    return pos;
}

QPointF GraphicsPiece::calcPosition(const QPoint &point) {
    QPointF pos(point.x() * scale_, point.y() * scale_);
    qDebug() << "calcPosition" << "point" << point << "calculated" << pos;
    return pos;
}

void GraphicsPiece::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverEnterEvent(event);
    setFocus();
    hovered_ = true;
    qDebug() << this << "hoverEnterEvent";
}
void GraphicsPiece::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsObject::hoverLeaveEvent(event);
    hovered_ = false;
    qDebug() << this << "hoverLeaveEvent";
}
//void GraphicsPiece::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
//    QGraphicsObject::hoverMoveEvent(event);
//    qDebug() << this << "hoverMoveEvent";
//}
void GraphicsPiece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mousePressEvent(event);
    pressed_ = true;
    update(); // update color
    qDebug() << this << "mousePressEvent" << event->button();
}
void GraphicsPiece::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() & Qt::LeftButton) {
        QGraphicsObject::mouseMoveEvent(event);
        return;
    }
    QPointF initial_mouse_pos = event->buttonDownScenePos(Qt::LeftButton);
    QPointF current_mouse_pos = event->scenePos();
    QPointF start_pos = calcPosition(piece_);
    QPointF mouse_move = current_mouse_pos - initial_mouse_pos;
    QPointF piece_move = QPointF(0, 0);
    if (std::abs(mouse_move.y()) > std::abs(mouse_move.x())) {
        if (mouse_move.y() < 0) {
            if (can_move_up) piece_move = QPointF(0, mouse_move.y());
        } else
            if (can_move_down) piece_move = QPointF(0, mouse_move.y());
        if (abs(piece_move.y()) > scale_)
            piece_move.setY(piece_move.y() > 0 ? scale_ : -scale_);
    } else {
        if (mouse_move.x() < 0) {
            if (can_move_left) piece_move = QPointF(mouse_move.x(), 0);
        } else {
            if (can_move_right) piece_move = QPointF(mouse_move.x(), 0);
        }
        if (abs(piece_move.x()) > scale_)
            piece_move.setX(piece_move.x() > 0 ? scale_ : -scale_);
    }
    qDebug() << "initial_mouse_pos" << initial_mouse_pos;
    qDebug() << "current_mouse_pos" << current_mouse_pos;
    qDebug() << "start_pos" << start_pos;
    qDebug() << "mouse_move" << mouse_move;
    qDebug() << "piece_move" << piece_move;
    setPos(start_pos + piece_move);
//    QGraphicsObject::mouseMoveEvent(event);
    qDebug() << this << "mouseMoveEvent" << event->button();
}
void GraphicsPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseReleaseEvent(event);
    pressed_ = false;
    update();
    applyMove(Move(-1, 0, 0));
    qDebug() << this << "mouseReleaseEvent" << event->button();
}
void GraphicsPiece::keyPressEvent(QKeyEvent *event) {
    QGraphicsObject::keyPressEvent(event);

    int x = 0, y = 0;
#ifdef IGNORE_VALID_MOVES
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:
        y = -1;
        break;
    case Qt::Key_S: case Qt::Key_Down:
        y = 1;
        break;
    case Qt::Key_A: case Qt::Key_Left:
        x = -1;
        break;
    case Qt::Key_D: case Qt::Key_Right:
        x = 1;
        break;
    }
#else
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:
        y = can_move_up ? -1 : 0;
        break;
    case Qt::Key_S: case Qt::Key_Down:
        y = can_move_down ? 1 : 0;
        break;
    case Qt::Key_A: case Qt::Key_Left:
        x = can_move_left ? -1 : 0;
        break;
    case Qt::Key_D: case Qt::Key_Right:
        x = can_move_right ? 1 : 0;
        break;
    }
#endif

    qDebug() << "Keypress move" << x << y;

    if (x != 0 || y != 0) {
        applyMove(Move(index_, x, y));
    }
    qDebug() << this << "keyPressEvent";
}

void GraphicsPiece::focusInEvent(QFocusEvent *event) {
    qDebug() << index_ << event;
    focused_ = true;
    // gain focus will auto update
}

void GraphicsPiece::focusOutEvent(QFocusEvent *event) {
    qDebug() << index_ << event;
    focused_ = false;
    update();
}

void GraphicsPiece::clearValidMoveDirection() {
    can_move_up = can_move_down = can_move_right = can_move_left = false;
}
void GraphicsPiece::addValidMoveDirection(const Move &valid_move) {
    if (valid_move.y() == -1)
        can_move_up = true;
    else if (valid_move.y() == 1)
        can_move_down = true;
    else if (valid_move.x() == -1)
        can_move_left = true;
    else if (valid_move.x() == 1)
        can_move_right = true;
}

void GraphicsPiece::applyMove(const Move &move) {
    static std::size_t emitted = 0;
    if (move.id() != emitted) {
        piece_ << move;

        QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
        animation->setStartValue(pos());
        QPointF end_position = calcPosition(piece_);
        animation->setEndValue(end_position);
        animation->setDuration((end_position - pos()).manhattanLength() / scale_ * 200);
        qDebug() << "Animation" << "start" << pos() << "end" << end_position;
        animation->start();

        qDebug() << "Move" << &move << "Finished on View";
        emitted = move.id();
        emit syncMove(move);
    } else {
        qDebug() << "Move" << &move << "required on View but have be down";
    }
}
