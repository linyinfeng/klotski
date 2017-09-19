//#define IGNORE_VALID_MOVES

#define MOVE_SYNC_THRESHOLD 0.6
//#define MOVE_DIRECTION_LIMIT_CANCEL_THRESHOLD 0.03

#include "common.h"
#include "graphicspiece.h"
#include "move.h"

#include <QPainter>
#include <QWidget>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QSequentialAnimationGroup>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QFont>
#include <cmath>

GraphicsPiece::GraphicsPiece(int index, const Piece &piece)
    : piece_(piece), index_(index)
{
    clearValidMoveDirection();

    hovered_ = false;
    pressed_ = false;
    focused_ = false;
    in_animation_ = false;
    have_skin_ = false;

    virtual_initial_mouse_pos_ = QPointF(0, 0);
    piece_base_pos_ = QPointF(0, 0);

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    qDebug() << "New GraphicsPiece" << index << piece_.geometry();
}

const Piece &GraphicsPiece::piece() const {
    return piece_;
}

int GraphicsPiece::index() const {
    return index_;
}

void GraphicsPiece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setBrush(background_brush_);
    painter->drawRoundedRect(rect_, scale_ * 0.05, scale_ * 0.05);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(Qt::black);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    if (focused_) {
        pen.setWidth(3);
    } else {
        pen.setWidth(2);
    }
    // Background
    painter->setPen(pen);
    if (pressed_)
        painter->setBrush(QColor(200, 200, 200, have_skin_ ? 64 : 128));
    else
        painter->setBrush(QColor(128, 128, 128, have_skin_ ? 64 : 128));
    painter->drawRoundedRect(rect_, scale_ * 0.05, scale_ * 0.05);

    // Arrows
    QRectF bounding_rect = boundingRect();
    if (!in_animation_) {
        if (focused_ || have_skin_) {
            painter->setBrush(QBrush(Qt::white));
        } else {
            painter->setBrush(QBrush(Qt::black));
        }
        pen.setWidth(0);
        painter->setPen(pen);
        qreal free_space = scale_ * 0.05;
        if (can_move_up_) {
            QPolygonF polygon;
            polygon << QPointF(bounding_rect.width() / 2, free_space * 2)
                    << QPointF(bounding_rect.width() / 2 - free_space * 2, free_space * 4)
                    << QPointF(bounding_rect.width() / 2 + free_space * 2, free_space * 4);
            painter->drawPolygon(polygon);
        }
        if (can_move_down_) {
            QPolygonF polygon;
            polygon << QPointF(bounding_rect.width() / 2, bounding_rect.height() - free_space * 2)
                    << QPointF(bounding_rect.width() / 2 - free_space * 2, bounding_rect.height() - free_space * 4)
                    << QPointF(bounding_rect.width() / 2 + free_space * 2, bounding_rect.height() - free_space * 4);
            painter->drawPolygon(polygon);
        }
        if (can_move_left_) {
            QPolygonF polygon;
            polygon << QPointF(free_space * 2, bounding_rect.height() / 2)
                    << QPointF(free_space * 4, bounding_rect.height() / 2 - free_space * 2)
                    << QPointF(free_space * 4, bounding_rect.height() / 2 + free_space * 2);
            painter->drawPolygon(polygon);
        }
        if (can_move_right_) {
            QPolygonF polygon;
            polygon << QPointF(bounding_rect.width() - free_space * 2, bounding_rect.height() / 2)
                    << QPointF(bounding_rect.width() - free_space * 4, bounding_rect.height() / 2 - free_space * 2)
                    << QPointF(bounding_rect.width() - free_space * 4, bounding_rect.height() / 2 + free_space * 2);
            painter->drawPolygon(polygon);
        }
    }
    // Text
    if (have_skin_) {
        pen.setColor(Qt::white);
    } else {
        pen.setColor(Qt::black);
    }
    painter->setPen(pen);
    static QFont font("Helvetica", 20);
    QFontMetrics font_metrics(font);
    QSize text_size = font_metrics.size(Qt::TextSingleLine, tr("%1").arg(index_));
    painter->setFont(font);
    painter->drawText(
        QPointF(bounding_rect.width() / 2 - text_size.width() / 2,
                bounding_rect.height() / 2 + text_size.height() / 2),
        QString("%1").arg(index_)
    );
}

void GraphicsPiece::setBackgroundImage(const QImage &image) {
    if (image.isNull()) {
        have_skin_ = false;
        background_image_ = image;
    } else {
        have_skin_ = true;
        background_image_ = image;
        scaleBackgroundImageToBrush();
    }
    update();
}

void GraphicsPiece::scaleBackgroundImageToBrush() {
    background_brush_ = background_image_.scaled(boundingRect().size().toSize(), Qt::KeepAspectRatioByExpanding);
}

QRectF GraphicsPiece::boundingRect() const {
    return QRectF(QPointF(0, 0), piece_.size() * scale_);
    // ignore free space for easy drawing
}

QRectF GraphicsPiece::calcRect(const Piece &piece) {
    qreal space_height = scale_ * piece.size().height();
    qreal space_width  = scale_ * piece.size().width();
    qreal free_space = scale_ * 0.05;
    QRectF res(0, 0, 0, 0);
    res.setSize(QSizeF(space_width - 2 * free_space, space_height - 2 * free_space));
    res.moveTopLeft(QPointF( free_space, free_space));
//    qDebug() << "calcRect" << "piece" << piece.geometry() << "calculated" << res;
    return res;
}
QPointF GraphicsPiece::calcPosition(const Piece &piece) {
    QPointF pos(piece.position().x() * scale_, piece.position().y() * scale_);
//    qDebug() << "calcPosition" << "piece" << piece.geometry() << "calculated" << pos;
    return pos;
}
QPointF GraphicsPiece::calcPosition(const QPoint &point) {
    QPointF pos(point.x() * scale_, point.y() * scale_);
//    qDebug() << "calcPosition" << "point" << point << "calculated" << pos;
    return pos;
}

void GraphicsPiece::onSceneResize() {
    if (scene() != nullptr) {
        scale_ = scene()->sceneRect().width() / kHorizontalUnit;
        rect_ = calcRect(piece_);
        piece_base_pos_ = calcPosition(piece_);
        setPos(piece_base_pos_);
        scaleBackgroundImageToBrush();

        qDebug() << "GraphicsPiece" << index_ << "Resize boundingRect" << boundingRect() << "pos" << pos();
        update();
    } else {
        qDebug() << "Invalid" << "scene_";
        rect_ = QRectF(0, 0, 0, 0);
    }
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
void GraphicsPiece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mousePressEvent(event);
    pressed_ = true;
    update(); // update color
    if (event->button() & Qt::LeftButton) {
//        moving_direction_ = Direction::invalid;
        virtual_initial_mouse_pos_ = event->scenePos();
    }
    qDebug() << this << "mousePressEvent" << event->button();
}
void GraphicsPiece::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() & Qt::LeftButton) {
        QGraphicsObject::mouseMoveEvent(event);
        return;
    }
    QPointF current_mouse_pos = event->scenePos();
    QPointF mouse_move = current_mouse_pos - virtual_initial_mouse_pos_;
    QPointF piece_move = QPointF(0, 0);

    if (std::abs(mouse_move.y()) > std::abs(mouse_move.x())) {

        if (mouse_move.y() < 0) {
            if (can_move_up_) {
                piece_move = QPointF(0, mouse_move.y());
                if (abs(piece_move.y()) > scale_ * MOVE_SYNC_THRESHOLD) {
                    applyMove(Move(index_, 0, -1), false);
                    virtual_initial_mouse_pos_.setY(virtual_initial_mouse_pos_.y() - scale_);
                    piece_move = QPointF(0, scale_ + mouse_move.y());
                }
            }
        } else
            if (can_move_down_) {
                piece_move = QPointF(0, mouse_move.y());
                if (abs(piece_move.y()) > scale_ * MOVE_SYNC_THRESHOLD) {
                    applyMove(Move(index_, 0, 1), false);
                    virtual_initial_mouse_pos_.setY(virtual_initial_mouse_pos_.y() + scale_);
                    piece_move = QPointF(0, -scale_ + mouse_move.y());
                }
            }
    } else {
        if (mouse_move.x() < 0) {
            if (can_move_left_) {
                piece_move = QPointF(mouse_move.x(), 0);
                if (abs(piece_move.x()) > scale_ * MOVE_SYNC_THRESHOLD) {
                    applyMove(Move(index_, -1, 0), false);
                    virtual_initial_mouse_pos_.setX(virtual_initial_mouse_pos_.x() - scale_);
                    piece_move = QPointF(scale_ + mouse_move.x(), 0);
                }
            }
        } else {
            if (can_move_right_) {
                piece_move = QPointF(mouse_move.x(), 0);
                if (abs(piece_move.x()) > scale_ * MOVE_SYNC_THRESHOLD) {
                    applyMove(Move(index_, 1, 0), false);
                    virtual_initial_mouse_pos_.setX(virtual_initial_mouse_pos_.x() + scale_);
                    piece_move = QPointF(- scale_ + mouse_move.x(), 0);
                }
            }
        }
//        if (abs(piece_move.x()) > scale_)
//            piece_move.setX(piece_move.x() > 0 ? scale_ : -scale_);
    }
    qDebug("%d, %d, %d, %d", can_move_up_, can_move_down_, can_move_left_, can_move_right_);
    qDebug() << "virtual_initial_mouse_pos_" << virtual_initial_mouse_pos_;
    qDebug() << "current_mouse_pos" << current_mouse_pos;
    qDebug() << "piece_base_pos_" << piece_base_pos_;
    qDebug() << "mouse_move" << mouse_move;
    qDebug() << "piece_move" << piece_move;
    setPos(piece_base_pos_ + piece_move);
    update();
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
        y = can_move_up_ ? -1 : 0;
        break;
    case Qt::Key_S: case Qt::Key_Down:
        y = can_move_down_ ? 1 : 0;
        break;
    case Qt::Key_A: case Qt::Key_Left:
        x = can_move_left_ ? -1 : 0;
        break;
    case Qt::Key_D: case Qt::Key_Right:
        x = can_move_right_ ? 1 : 0;
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
    can_move_up_ = can_move_down_ = can_move_right_ = can_move_left_ = false;
    qDebug() << "Piece" << index_ << "clearValidMoveDirection";
    update();
}
void GraphicsPiece::addValidMoveDirection(const Move &valid_move) {
    qDebug() << "Piece" << index_ << "adding valid move direction";
    if (valid_move.y() == -1)
        can_move_up_ = true;
    else if (valid_move.y() == 1)
        can_move_down_ = true;
    else if (valid_move.x() == -1)
        can_move_left_ = true;
    else if (valid_move.x() == 1)
        can_move_right_ = true;
    update();
}

void GraphicsPiece::applyMove(const Move &move, bool animate) {
    static std::size_t emitted = 0;
    if (move.id() != emitted) {
        piece_ << move;
        QPointF last_piece_base_pos = piece_base_pos_;
        piece_base_pos_ = calcPosition(piece_);

        if (animate) {
            QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
            animation->setEndValue(piece_base_pos_);
            animation->setDuration(200);
            if (move.isNull()) {
                animation->setStartValue(pos());
                animation->start(QPropertyAnimation::DeleteWhenStopped);
            } else {
                animation->setStartValue(last_piece_base_pos);
                addAnimation(animation);
            }
        }

        qDebug() << "Move" << &move << "Finished on View";

        if (move.index() != -1) {
            emitted = move.id();
            qDebug() << "[EMIT] syncMove(move)";
            emit syncMove(move);
        }
    } else {
        qDebug() << "Move" << &move << "required on View but have be down";
    }
}

void GraphicsPiece::animationFinished() {
    in_animation_ = false;
    update();
}
void GraphicsPiece::animationStarted() {
    in_animation_ = true;
    // auto update by animation
}
