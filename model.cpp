#include "model.h"
#include "common.h"
#include "matrix.h"

#include <chrono>
#include <cstddef>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QTextCodec>

Model::Model(QObject *parent) : QObject(parent), history_model_(this) { }

void Model::onViewRequireDataRefresh() {
    qDebug() << "[EMIT] emit piecesChanged(pieces_)";
    emit piecesChanged(pieces_);
    qDebug() << "[EMIT] updateValidMoves()";
    updateValidMoves(); // auto emitted
    qDebug() << "[EMIT] historyModelChanged(&history_model_)";
    emit historyModelChanged(&history_model_);
    qDebug() << "[EMIT] emit currentMoveIndexChanged(current_move_index_)";
    emit currentMoveIndexChanged(current_move_index_);
    qDebug() << "[EMIT] emit levelNameChanged(level_name_)";
    emit levelNameChanged(level_name_);
    qDebug() << "[EMIT] emit bestStepCountChanged(best_step_count_)";
    emit bestStepCountChanged(best_step_count_);
    qDebug() << "[EMIT] emit stepCountChanged(step_count_)";
    emit stepCountChanged(step_count_);
    qDebug() << "[EMIT] updateCanUndoRedoState()";
    updateCanUndoRedoState();
    qDebug() << "[EMIT] updateCanWinState()";
    updateCanWinState();
}

void Model::onSaveToFile(const QString & file_name){
    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
        stream.setGenerateByteOrderMark(true);
        stream << level_name_ << "\n";
        stream << best_step_count_<<"\n";
        stream << static_cast<int>(pieces_.size()) << "\n";
        for(int i = 0; i < static_cast<int>(pieces_.size()); ++i)
        {
            stream << i << "\t" << pieces_[i].position().x() << "\t" << pieces_[i].position().y()
                   << "\t" << pieces_[i].size().width() << "\t" << pieces_[i].size().height() << "\n";
        }
        qDebug() << "[emit] savedToFile(true)";
        emit savedToFile(true);
    } else
        qDebug() << "[emit] savedToFile(false)";
        emit savedToFile(false);

}
void Model::onLoadFile(const QString & file_name){
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
        onReset();
    } else {
        qDebug() << "Failed to open file";
    }
}
void Model::onReset() {
    qDebug() << "Model::onReload";
    pieces_ = original_pieces_;
    history_model_.reset();
    current_move_index_ = -1;
    step_count_ = 0;

    onViewRequireDataRefresh();
//    emit modelReset(this);
//    updateCanUndoRedoState();
//    updateCanWinState();
//    setStepCount(0);
//    updateValidMoves();
}

void Model::applyMove(const Move &move) {
    static std::size_t emitted = 0;
    if (move.id() != emitted) {
        emitted = move.id();
        qDebug() << "[emit] syncMove(move)";
        emit syncMove(move);

        /* History Logic */
        // Update history_ step_count_ valid_moves_ pieces_ history_model->last_move_
        if (current_move_index_ != -1 && move == history_model_[current_move_index_].reverse()) {
            // Undo
            decCurrentMoveIndex();
            if (history_model_[current_move_index_].index() != history_model_[current_move_index_ + 1].index())
                // current_move_index_ has been decreased
                decStepCount();
        } else {
            if (current_move_index_ + 1 < history_model_.rowCount() && move == history_model_[current_move_index_ + 1]) {
                // Redo
                // history_ unchanged

                if (current_move_index_ <= 0 ||
                    history_model_[current_move_index_].index() !=
                        history_model_[current_move_index_ - 1].index())
                    incStepCount();
                incCurrentMoveIndex();
            } else {
                // Move forward
                if (current_move_index_ == -1 ||
                    move.index() != history_model_[current_move_index_].index())
                    incStepCount();
                history_model_.cutToFit(current_move_index_ + 1);
                qDebug() << "push move to history_model_";
                history_model_.pushBack(step_count_, move);
                incCurrentMoveIndex();
            }
        }
        updateCanUndoRedoState();

        /* Game Logic */
        pieces_[move.index()] << move;
        updateValidMoves();
        updateCanWinState();

        qDebug() << "Move" << &move << "Finished on Model";
    } else {
        qDebug() << "Move" << &move << "required on Model but have be down";
    }
}
void Model::onUndo() {
    applyMove(history_model_[current_move_index_].reverse());
}
void Model::onRedo() {
    applyMove(history_model_[current_move_index_ + 1]);
}

void Model::onUserSelectedHistory(int selected) {
    if (selected == current_move_index_) {
        return;
    }
    int current_move_index = current_move_index_;
    if (selected > current_move_index) {
        // Redo
        for (int i = current_move_index + 1; i <= selected; ++i) {
            applyMove(history_model_[i]);
        }
    } else {
        // Undo
        for (int i = current_move_index; i >= selected + 1; --i) {
            applyMove(history_model_[i].reverse());
        }
    }
}

void Model::updateValidMoves() {
    valid_moves_.clear();
    Matrix<int> matrix(kHorizontalUnit, kVerticalUnit);
for(int i = 0; i < static_cast<int>(pieces_.size()); i++)
 {
    int x, y;
    x = pieces_[i].position().x();
    y = pieces_[i].position().y();
    for(int j = x; j <= x + pieces_[i].size().width() - 1; j++){
        for(int k = y; k <= y + pieces_[i].size().height() - 1; k++){
            matrix.at(j, k) = 1;
        }
   }
}
for(int i = 0; i < static_cast<int>(pieces_.size()); i++)
    {
        int x, y;
        x = pieces_[i].position().x();
        y = pieces_[i].position().y();
        int width, height;
        width = pieces_[i].size().width();
        height = pieces_[i].size().height();

    if(y>0){
    bool canup = true;//move up
    for (int j = 0; j <= width - 1; j++) {
        if (matrix.at(x + j, y - 1) != 0)
        {
            canup = false;
            break;
        }
    }
    if (canup)
        valid_moves_.push_back(Move(i, 0, -1));
   }

    if(y < kVerticalUnit - height){
    bool candown = true;//move down
    for (int j = 0; j <= width - 1; j++)
   {
        if (matrix.at(x + j, y + height) != 0)
        {
            candown = false;
            break;
        }
   }
    if (candown)
        valid_moves_.push_back(Move(i, 0, 1));
    }

    if(x > 0){
    bool canleft = true;//move left
    for (int j = 0; j <= height - 1; j++) {
        if (matrix.at(x - 1, y + j) != 0)
        {
            canleft = false;
            break;
        }
    }
    if (canleft)
        valid_moves_.push_back(Move(i, -1, 0));
 }
    if(x + width < kHorizontalUnit){
        bool canright = true;
        for(int j = 0; j <= height - 1; j++)
        {
            if(matrix.at(x + width, y + j) != 0){
                canright = false;
                break;
            }
        }
        if(canright)
            valid_moves_.push_back(Move(i, 1, 0));
    }
    }
    emit validMovesChanged(valid_moves_);
}
void Model::updateCanWinState() {
    emit canWinStateChanged(pieces_[kWinPieceIndex].position() == kWinPosition);
}
void Model::updateCanUndoRedoState() {
    bool can_undo = false, can_redo = false;
    if (current_move_index_ != -1) {
        can_undo = true;
    }
    if (current_move_index_ + 1 < history_model_.rowCount()) {
        // can push more records in
        can_redo = true;
    }
    qDebug() << "[EMIT] canUndoStateChanged(can_undo)";
    emit canUndoStateChanged(can_undo);
    qDebug() << "[EMIT] canRedoStateChanged(can_redo)";
    emit canRedoStateChanged(can_redo);
}

void Model::incStepCount() {
    setStepCount(step_count_ + 1);
}
void Model::decStepCount() {
    setStepCount(step_count_ - 1);
}
void Model::setStepCount(int step_count) {
    step_count_ = step_count;
    qDebug() << "[emit] stepCountChanged(step_count_)";
    emit stepCountChanged(step_count_);
}
void Model::incCurrentMoveIndex() {
    setCurrentMoveIndex(current_move_index_ + 1);
}
void Model::decCurrentMoveIndex() {
    setCurrentMoveIndex(current_move_index_ - 1);
}
void Model::setCurrentMoveIndex(int current_move) {
    current_move_index_ = current_move;
    qDebug() << "[emit] currentMoveIndexChanged(current_move_index_)";
    emit currentMoveIndexChanged(current_move_index_);
}
