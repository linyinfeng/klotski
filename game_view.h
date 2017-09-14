#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "game_model.h"
#include "game_view.h"
#include "game_piece_view.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHoverEvent>

namespace game {

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = nullptr);

public:
    // Read
    const Model *model() const { return m_model; }
    float scale()  const { return m_scale; }

protected:
    virtual void resizeEvent(QResizeEvent *event);

public slots:
    // Write
    void setModel(const Model *model);

    void setScale(float scale);

    void onModelReady();

signals:
    void scaleChanged(float new_scale);

private:
    // Member
    QGraphicsScene *m_scene;
    float m_scale;

    std::vector<PieceView *> m_piece_views;

    void createNewPiece(const QRect &rect);
    void clearScene();

private:
    // Model
    const Model *m_model;

protected:
    virtual void enterEvent(QEvent *event) override;
};

}

#endif // GAME_VIEW_H
