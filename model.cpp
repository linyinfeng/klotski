#include "model.h"
#include "common.h"
#include "matrix.h"

#include <cstddef>
#include <QDebug>
#include <QFile>
#include <QTextStream>

Model::Model(QObject *parent) : QObject(parent) {
    load(QString(":/resources/levels/QiBuChengShi.klotski"));
    // Wait for reload
}
const std::vector<Piece> &Model::pieces() const {
    return pieces_;
}
const std::vector<Move> &Model::validMoves() const {
    return valid_moves_;
}
int Model::stepCount() const {
    return step_count_;
}
int Model::bestStepCount() const {
    return best_step_count_;
}
const QString &Model::levelName() const {
    return level_name_;
}

void Model::onSave(const QString & file_name){
    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << level_name_ << "\n";
        stream << best_step_count_<<"\n";
        stream<< static_cast<int>(pieces_.size()) << "\n";
        for(int i = 0; i < static_cast<int>(pieces_.size()); ++i)
        {
            stream << i << "\t" << pieces_[i].position().x() << "\t" << pieces_[i].position().y()
                   << "\t" << pieces_[i].size().width() << "\t" << pieces_[i].size().height() << "\n";
        }
        emit modelSaved(true);
    } else
        emit modelSaved(false);

}
void Model::onLoad(const QString & file_name){
    load(file_name);
    onReload();
}
void Model::onReload() {
    qDebug() << "Model::onReload";
    pieces_ = original_pieces_;
    history_.clear();
    last_move_ = -1;
    setStepCount(0);
    updateCanUndoRedoState();
    updateCanWinState();
    emit modelLoaded(this);
    updateValidMoves();
}
void Model::load(const QString & file_name) {
    qDebug() << "Start Load";
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly))
    {
        original_pieces_.clear();
        QTextStream stream(&file);
        int pieces_count;
        int index, x, y, width, height;
        stream >> level_name_;
        stream >> best_step_count_;
        stream >> pieces_count;
        for(int i = 0; i < pieces_count; ++i)
        {
            stream >> index >> x >> y >> width >> height;
            original_pieces_.push_back(QRect(x,y,width,height));
        }

        qDebug() << "Load finish";
    } else {
        qDebug() << "Failed to open file";
    }
}

void Model::applyMove(const Move &move) {
    static std::size_t emitted = 0;
    if (move.id() != emitted) {
        emitted = move.id();
        emit syncMove(move);

        // Update history_ step_count_ valid_moves_ pieces_ last_move_
        if (last_move_ != -1 && move == history_[last_move_].reverse()) {
            // Undo
            // history_ unchanged
            --last_move_;
            if (history_[last_move_].index() != history_[last_move_ + 1].index())
                decStepCount();
        } else {
            if (last_move_ + 1 < static_cast<int>(history_.size()) && move == history_[last_move_ + 1]) {
                // Redo
                // history_ unchanged
            } else {
                // Move forward
                history_.resize(last_move_ + 1);
                history_.push_back(move);
            }
            ++last_move_;
            if (last_move_ <= 0 || history_[last_move_].index() != history_[last_move_ - 1].index())
                incStepCount();
        }
        updateCanUndoRedoState();
        pieces_[move.index()] << move;
        updateValidMoves();
        updateCanWinState();

        qDebug() << "Move" << &move << "Finished on Model";
    } else {
        qDebug() << "Move" << &move << "required on Model but have be down";
    }
}
void Model::onUndo() {
    applyMove(history_[last_move_].reverse());
}
void Model::onRedo() {
    applyMove(history_[last_move_ + 1]);
}

void Model::updateValidMoves() {
    valid_moves_.clear();

    Matrix<int> matrix(kHorizontalUnit, kVerticalUnit);

    for (int i = 0; i < static_cast<int>(pieces_.size()); i++)
    {
        int x, y;
        x = pieces_[i].position().x();
        y = pieces_[i].position().y();
        matrix.at(x, y) = 1;
        if (pieces_[i].size().width() == 2)
            matrix.at(x + 1, y) = 1;
        if (pieces_[i].size().height() == 2)
            matrix.at(x, y + 1) = 1;
        if (pieces_[i].size().width() == 2 && pieces_[i].size().height() == 2)
            matrix.at(x + 1, y + 1) = 1;
    }

    for (int i = 0; i < static_cast<int>(pieces_.size()); i++)
    {
        if (pieces_[i].size().width() == 1 && pieces_[i].size().height() == 1)//1*1
        {
            if (pieces_[i].position().x() > 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y()) == 0)
                valid_moves_.push_back(Move(i, -1, 0));//move left

            if (pieces_[i].position().x() < kHorizontalUnit - 1 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y()) == 0)
                valid_moves_.push_back(Move(i, 1, 0));//move right

            if (pieces_[i].position().y() > 0 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() - 1) == 0)
                valid_moves_.push_back(Move(i, 0, -1));//move up

            if (pieces_[i].position().y() < kVerticalUnit - 1 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, 0, 1));//move down
        }

        if (pieces_[i].size().width() == 1 && pieces_[i].size().height() == 2)//1*2
        {
            if (pieces_[i].position().x() > 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y()) == 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, -1, 0));//move left

            if (pieces_[i].position().x() < kHorizontalUnit - 1 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y()) == 0 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, 1, 0));//move right

            if (pieces_[i].position().y() > 0 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() - 1) == 0)
                valid_moves_.push_back(Move(i, 0, -1));//move up

            if (pieces_[i].position().y() < kVerticalUnit - 2 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() + 2) == 0)
                valid_moves_.push_back(Move(i, 0, 1));//move down
        }

        if (pieces_[i].size().width() == 2 && pieces_[i].size().height() == 1)//2*1
        {
            if (pieces_[i].position().x() > 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y()) == 0)
                valid_moves_.push_back(Move(i, -1, 0));//move left

            if (pieces_[i].position().x() < kHorizontalUnit - 2 &&
                matrix.at(pieces_[i].position().x() + 2, pieces_[i].position().y()) == 0)
                valid_moves_.push_back(Move(i, 1, 0));//move right

            if (pieces_[i].position().y() > 0 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() - 1) == 0 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y() - 1) == 0)
                valid_moves_.push_back(Move(i, 0, -1));//move up

            if (pieces_[i].position().y() < kVerticalUnit - 1 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() + 1) == 0 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, 0, 1));//move down
        }

        if (pieces_[i].size().width() == 2 && pieces_[i].size().height() == 2)//2*2
        {
            if (pieces_[i].position().x() > 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y()) == 0 &&
                matrix.at(pieces_[i].position().x() - 1, pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, -1, 0));//move left

            if (pieces_[i].position().x() < kHorizontalUnit - 2 &&
                matrix.at(pieces_[i].position().x() + 2, pieces_[i].position().y()) == 0 &&
                matrix.at(pieces_[i].position().x() + 2, pieces_[i].position().y() + 1) == 0)
                valid_moves_.push_back(Move(i, 1, 0));//move right

            if (pieces_[i].position().y() > 0 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() - 1) == 0 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y() - 1) == 0)
                valid_moves_.push_back(Move(i, 0, -1));//move up

            if (pieces_[i].position().y() < kVerticalUnit - 2 &&
                matrix.at(pieces_[i].position().x(), pieces_[i].position().y() + 2) == 0 &&
                matrix.at(pieces_[i].position().x() + 1, pieces_[i].position().y() + 2) == 0)
                valid_moves_.push_back(Move(i, 0, 1));//move down
        }
    }
    emit validMovesChanged(valid_moves_);
}
void Model::updateCanWinState() {
    emit canWinStateChanged(pieces_[kWinPieceIndex].position() == kWinPosition);
}
void Model::updateCanUndoRedoState() {
    bool can_undo = false, can_redo = false;
    if (last_move_ != -1) {
        can_undo = true;
    }
    if (last_move_ + 1 < static_cast<int>(history_.size())) {
        can_redo = true;
    }
    emit canUndoStateChanged(can_undo);
    emit canRedoStateChanged(can_redo);
}

void Model::incStepCount() {
    emit stepCountChanged(++step_count_);
    qDebug() << "Step Count Changed emitted" << step_count_;
}
void Model::decStepCount() {
    emit stepCountChanged(--step_count_);
    qDebug() << "Step Count Changed emitted" << step_count_;
}
void Model::setStepCount(int step_count) {
    step_count_ = step_count;
    emit stepCountChanged(step_count_);
    qDebug() << "Step Count Changed emitted" << step_count_;
}
