#include "game_controller.h"

#include <QObject>
#include <vector>

namespace game {

Controller::Controller(QObject *parent) : QObject(parent)
{
    // Setup window
    m_window = new MainWindow;
    // Read model
    loadModelData();
}

void Controller::loadModelData() {
    // TODO
    std::vector<Piece> pieces;
    pieces.push_back(QRect(1, 0, 2, 2));

    pieces.push_back(QRect(0, 0, 1, 2));
    pieces.push_back(QRect(3, 0, 1, 2));
    pieces.push_back(QRect(0, 2, 1, 2));
    pieces.push_back(QRect(3, 2, 1, 2));

    pieces.push_back(QRect(1, 2, 2, 1));

    pieces.push_back(QRect(0, 4, 1, 1));
    pieces.push_back(QRect(1, 3, 1, 1));
    pieces.push_back(QRect(2, 3, 1, 1));
    pieces.push_back(QRect(3, 4, 1, 1));

    m_model = new Model(this, 4, 5, 0, pieces);
    if (m_window) {
        m_window->setModel(m_model);
    }
    generateLayout();
    emit modelChanged(m_model);
}

void Controller::generateLayout() {
    if (layout)
        delete layout;
    layout = new Matrix<int>(m_model->horizontalUnit(), m_model->verticalUint());
    for (int i = 0; i < m_model->piecesNum(); ++i) {
        const Piece &piece = m_model->piece(i);
        layout->at(piece.position().x(), piece.position().y()) = i;
    }
    debugOutputLayout();
}

void Controller::debugOutputLayout() {
    qDebug() << "Debug output layout" << layout;
    for (int i = 0; i < m_model->horizontalUnit(); ++i) {
        for (int j = 0; j < m_model->verticalUint(); ++j) {
            qDebug().noquote() << layout->at(i, j);
        }
        qDebug() << "LINE" << i;
    }
}

void Controller::performMove(const Move &move)
{
    Q_UNUSED(move);
}

void Controller::undo()
{

}

void Controller::redo()
{

}

}
