#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "game_facilities.h"

#include <vector>
#include <QRect>
#include <QObject>

namespace game {

class Move;
class Piece;

class Model : public QObject {
    Q_OBJECT
public:
    Model(QObject *parent, int horizontal_unit, int vertical_unit, int step_count, const std::vector<Piece> &pieces);

    int stepCount()      const { return m_step_count;      }
    int horizontalUnit() const { return m_horizontal_unit; }
    int verticalUint()   const { return m_vertical_unit;   }

    const std::vector<Piece> &pieces() const { return m_pieces;  }
    const std::vector<Move>  &validMoves() const{
        return m_valid_moves;
    }

    int piecesNum() const { return m_pieces.size(); }

    const Piece &piece(int index) const;

signals:
    void pieceChanged(int index);
    void stepCountChanged(int step_count);

public slots:
    void setPiece(int index, const Piece &piece);

    void setStepCount(int step_count);
    void setValidMoves(const std::vector<Move> &moves);
    void incStepCount();
    void decStepCount();

    void performMove(const Move &move);

private:
    bool checkMove(const Move &move) const;
    bool checkIndex(int index) const;

    const int m_horizontal_unit;
    const int m_vertical_unit;
    int m_step_count;

    std::vector<Piece> m_pieces;
    std::vector<Move> m_valid_moves;
};

} // namespace game

#endif // GAME_MODEL_H
