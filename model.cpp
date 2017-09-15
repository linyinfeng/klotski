#include "model.h"
#include "common.h"
#include "matrix.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

Model::Model(QObject *parent) : QObject(parent) {
    best_step_count_ = 81;
    level_name_ = tr("HengDaoLiMa");
    original_pieces_ = std::vector<Piece>({
        QRect(1, 0, 2, 2), // Main piece
        QRect(0, 0, 1, 2),
        QRect(3, 0, 1, 2),
        QRect(0, 2, 1, 2),
        QRect(3, 2, 1, 2),
        QRect(1, 2, 2, 1),
        QRect(0, 4, 1, 1),
        QRect(3, 4, 1, 1),
        QRect(1, 3, 1, 1),
        QRect(2, 3, 1, 1)
    });
    pieces_ = original_pieces_;
    clearHistory();
    setStepCount(0);
    updateCanUndoRedoState();
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
void Model::onSyncMove(const Move &move) {
    // When onMove, view has finish its update
    if (last_move_ != -1 && move.reverse() == history_[last_move_]) {
        undoWithOutSync();
        return;
    }
    if (last_move_ + 1 != static_cast<int>(history_.size())) {
        history_.resize(last_move_ + 1);
    }
    if (last_move_ == -1 || history_[last_move_].index() != move.index())
        incStepCount();
    history_.push_back(move);
    ++last_move_;
    applyMove(move);

    updateCanUndoRedoState();
}
void Model::undoWithOutSync() {
    const Move &move = history_[last_move_];
    --last_move_;
    applyMove(move.reverse());

    if (last_move_ == -1 || move.index() != history_[last_move_].index())
        decStepCount();

    updateCanUndoRedoState();
    updateLogic();
}
void Model::onUndo() {
    if (last_move_ == -1) {
        qDebug() << "Error: Can't undo";
    } else {
        Move move = history_[last_move_];
        undoWithOutSync();
        syncMove(move.reverse());
    }
}
void Model::onRedo() {
    if (last_move_ + 1 < static_cast<int>(history_.size())) {
        ++last_move_;
        moveAndSync(history_[last_move_]);
        emit canUndoStateChanged(true);
        if (last_move_ + 1 == static_cast<int>(history_.size())) {
            emit canRedoStateChanged(false);
        }

        if (last_move_ == -1 || history_[last_move_ - 1].index() != history_[last_move_].index())
            incStepCount();
    } else {
        qDebug() << "Error: Can't redo";
    }
    updateCanUndoRedoState();
}
void Model::onReload() {
    qDebug() << "Model::onReload";
    pieces_ = original_pieces_;
    clearHistory();
    setStepCount(0);
    updateCanUndoRedoState();
    emit modelLoaded(this);
    updateLogic();
}
void Model::clearHistory() {
    history_.clear();
    last_move_ = -1;

    updateCanUndoRedoState();
}
void Model::applyMove(const Move &move) {
    pieces_[move.index()] << move;
    updateLogic();
}
void Model::moveAndSync(const Move &move) {
    emit syncMove(move);
    applyMove(move);
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
void Model::updateLogic() {
    updateValidMoves();
    updateCanWinState();
}
void Model::updateValidMoves() {
    valid_moves_.clear();

    Matrix<int> matrix(kHorizontalUnit, kVerticalUnit);
    for (int i = 0; i < kHorizontalUnit; i++)//set matrix with 0
    {
        for (int j = 0; j < kVerticalUnit; j++) {
            matrix.at(i, j) = 0;
        }
    }

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
    QFile file(file_name);
    original_pieces_.clear();
    if(file.open(QIODevice::ReadOnly))
    {
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
    }
    onReload();
    qDebug() << "Model::onLoad()";
}
