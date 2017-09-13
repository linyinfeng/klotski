#include "gameview.h"
#include "gamecommon.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>
#include <QTimer>

GameView::GameView(QWidget *parent)
    : QWidget(parent),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this))
{
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setBackgroundBrush(Qt::white);
}

void GameView::onGameViewResized()
{
    m_scene->setSceneRect(0, 0, width(), height());
    m_view->setGeometry(0, 0, width(), height());
    m_view->fitInView(QRect(0, 0, width(), height()));
    qDebug() << this << "resized...";
}

GameModel *GameView::model()
{
    return m_model;
}

void GameView::setModel(GameModel *model)
{
    m_model = model;
}
