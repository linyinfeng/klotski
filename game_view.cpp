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

void View::setModel(const Model *model) {
    if (m_model) {
        clearScene();
    }
    m_model = model;
    onModelReady();
}

void View::setScale(float scale) {
    m_scale = scale;
    emit scaleChanged(m_scale);
}

void View::clearScene() {
    for (PieceView *piece_view : m_piece_views) {
        m_scene->removeItem(piece_view);
        piece_view->deleteLater();
    }
}

void View::onModelReady() {
    // gen piece_views
    const std::vector<Piece> pieces = m_model->pieces();
    for (const Piece &piece : pieces) {
        PieceView *piece_view = new PieceView(&piece, m_scale);
        m_piece_views.push_back(piece_view);
        m_scene->addItem(piece_view);
    }
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
