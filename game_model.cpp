#include "game_facilities.h"
#include "game_model.h"
#include <vector>
#include <QDebug>

namespace game {

Model::Model(QObject *parent,
             int horizontal_unit,
             int vertical_unit,
             int step_count,
             const std::vector<Piece> &pieces
) : QObject(parent),
      m_horizontal_unit(horizontal_unit),
      m_vertical_unit(vertical_unit),
      m_step_count(step_count),
      m_pieces(pieces)
{
}

const Piece &Model::piece(int index) const {
    // index check with at
    return m_pieces.at(index);
}

void Model::setPiece(int index, const Piece &piece) {
    if (checkIndex(index)) {
        m_pieces[index] = piece;
        emit pieceChanged(index);
    } else {
        qDebug() << "Invalid piece" << index;
    }
}

void Model::setStepCount(int step_count) {
    m_step_count = step_count;
    emit stepCountChanged(step_count);
}

void Model::setValidMoves(const std::vector<Move> &moves) {
    m_valid_moves = moves;
}

void Model::incStepCount() {
    emit stepCountChanged(++m_step_count);
}

void Model::decStepCount() {
    emit stepCountChanged(--m_step_count);
}

void Model::performMove(const Move &move) {
    if (checkMove(move))
    m_pieces[move.index()] << move;
    emit pieceChanged(move.index());
}

bool Model::checkMove(const Move &move) const {
    return checkIndex(move.index());
}

bool Model::checkIndex(int n) const {
    return n > 0 && n < piecesNum();
}

}
