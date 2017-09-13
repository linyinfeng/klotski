#include "gameview.h"
#include "gamecommon.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>
#include <QTimer>

GameView::GameView(QWidget *parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene(this))
{
    setAttribute(Qt::WA_Hover, true);

    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);

    createNewPiece(QRect(0, 0, 1, 2));
    createNewPiece(QRect(1, 0, 2, 2));
    createNewPiece(QRect(3, 0, 1, 2));
    createNewPiece(QRect(0, 2, 1, 2));
    createNewPiece(QRect(1, 2, 2, 1));
    createNewPiece(QRect(3, 2, 1, 2));
    createNewPiece(QRect(0, 4, 1, 1));
    createNewPiece(QRect(3, 4, 1, 1));
    createNewPiece(QRect(1, 3, 1, 1));
    createNewPiece(QRect(2, 3, 1, 1));

//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, m_scene, &QGraphicsScene::advance);
//    timer->start(16);
}

void GameView::createNewPiece(const QRect &rect)
{
    GraphicsPiece *piece = new GraphicsPiece(nullptr, rect, m_scale);
    connect(this, &GameView::scaleChanged, piece, &GraphicsPiece::setScale);
    pieces.push_back(piece);
    m_scene->addItem(piece);
}

void GameView::onGameViewResized()
{
    QRect scene_rect(0, 0, width(), height());
    m_scene->setSceneRect(scene_rect);
    fitInView(scene_rect);

    setScale(static_cast<float>(scene_rect.width()) / HorizontalUnit);
}

void GameView::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
    setFocus(Qt::MouseFocusReason);
    qDebug() << "GameView::enterEvent";
}
