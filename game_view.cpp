#include "common.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTimer>

namespace game {

View::View(QWidget *parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene(this))
{
    setAttribute(Qt::WA_Hover, true);

    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);
}

//void View::createNewPiece(const QRect &rect)
//{
//    PieceView *piece = new PieceView(nullptr, rect, m_scale);
//    connect(this, &View::scaleChanged, piece, &PieceView::setScale);
//    pieces.push_back(piece);
//    m_scene->addItem(piece);
//}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    QRect scene_rect(0, 0, width(), height());
    m_scene->setSceneRect(scene_rect);
    fitInView(scene_rect);

    setScale(static_cast<float>(scene_rect.width()) / HorizontalUnit);
}

void View::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
    setFocus(Qt::MouseFocusReason);
    qDebug() << "GameView::enterEvent";
}

}
