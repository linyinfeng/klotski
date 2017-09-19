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

Model::Model(QObject *parent)
    : QObject(parent),
      level_name_(),
      best_step_count_(0),
      pieces_(),
      original_pieces_(),
      valid_moves_(),
      step_count_(),
      history_model_(this),
      current_move_index_(-1)
{ }

void Model::onViewRequireDataRefresh() {
    qDebug() << "[EMIT] emit piecesChanged(pieces_)";
    emit piecesChanged(pieces_);
    qDebug() << "[EMIT] updateValidMoves()";
    updateValidMoves(); // auto emitted
    qDebug() << "[EMIT] historyModelChanged(&history_model_)";
    emit historyModelChanged(&history_model_);
    qDebug() << "[EMIT] currentMoveIndexChanged(current_move_index_)";
    emit currentMoveIndexChanged(current_move_index_);
    qDebug() << "[EMIT] levelNameChanged(level_name_)";
    emit levelNameChanged(level_name_);
    qDebug() << "[EMIT] fileNameChanged(file_name_)";
    emit fileNameChanged(file_name_);
    qDebug() << "[EMIT] bestStepCountChanged(best_step_count_)";
    emit bestStepCountChanged(best_step_count_);
    qDebug() << "[EMIT] stepCountChanged(step_count_)";
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
        // Level Basic Infomation
        stream << level_name_ << "\n";
        stream << best_step_count_ << "\n";
        stream << static_cast<int>(pieces_.size()) << "\n";
        for (int i = 0; i < static_cast<int>(pieces_.size()); ++i)
        {
            stream << i << "\t" << original_pieces_[i] << "\n";
        }
        stream << "\n";
        for (int i = 0; i < static_cast<int>(pieces_.size()); ++i)
        {
            stream << i << "\t" << pieces_[i] << "\n";
        }
        stream << "\n";
        stream << step_count_ << "\n";
        int history_count = history_model_.rowCount();
        stream << history_count << "\n";
        for (int i = 0; i < history_count; ++i) {
            const Move &move = history_model_[i].second;
            stream << history_model_[i].first << "\t" // step
                   << move << "\n";
        }
        stream << current_move_index_ << "\n";

        stream << flush;
        qDebug() << "[emit] savedToFile(true)";
        emit savedToFile(true);
    } else {
        qDebug() << "[emit] savedToFile(false)";
        emit savedToFile(false);
    }
}
void Model::onLoadFile(const QString & file_name){
    qDebug() << "Start Load";
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly))
    {    
        original_pieces_.clear();
        pieces_.clear();
        history_model_.reset();

        QTextStream stream(&file);

        stream >> level_name_;
        stream >> best_step_count_;
        int pieces_count;
        stream >> pieces_count;
        int index, x, y, width, height;

        for(int i = 0; i < pieces_count; ++i)
        {
            stream >> index >> x >> y >> width >> height;
            original_pieces_.push_back(QRect(x, y, width, height));
        }

        for(int i = 0; i < pieces_count; ++i)
        {
            stream >> index >> x >> y >> width >> height;
            pieces_.push_back(QRect(x, y, width, height));
        }

        stream >> step_count_;
        int history_count;
        stream >> history_count;
        for (int i = 0; i < history_count; ++i) {
            int step_count;
            Move move;
            stream >> step_count >> move;
            history_model_.pushBack(step_count, move);
        }
        stream >> current_move_index_;

        qDebug() << "Start refresh";
        file_name_ = file_name;
        onViewRequireDataRefresh();
    } else {
        qDebug() << "Failed to open file";
    }
}
void Model::onReset() {
    qDebug() << "Model::onReload";
    pieces_ = original_pieces_;
    history_model_.reset();
    current_move_index_ = -1;
    valid_moves_.clear();
    step_count_ = 0;

    onViewRequireDataRefresh();
}
void Model::applyMove(const Move &move) {
    static std::size_t emitted = 0;
    if (move.id() != emitted) {
        emitted = move.id();
        qDebug() << "[emit] syncMove(move)";
        emit syncMove(move);

        /* History Logic */
        // Update history_ step_count_ valid_moves_ pieces_ history_model->last_move_
        if (current_move_index_ != -1 && move == history_model_[current_move_index_].second.reverse()) {
            // Undo
            decCurrentMoveIndex();
            if (history_model_[current_move_index_].second.index() != history_model_[current_move_index_ + 1].second.index())
                // current_move_index_ has been decreased
                decStepCount();
        } else {
            if (current_move_index_ + 1 < history_model_.rowCount() && move == history_model_[current_move_index_ + 1].second) {
                // Redo
                // history_ unchanged
                incCurrentMoveIndex();
                if (current_move_index_ <= 0 ||
                    history_model_[current_move_index_].second.index() !=
                        history_model_[current_move_index_ - 1].second.index())
                    incStepCount();
            } else {
                // Move forward
                if (current_move_index_ == -1 ||
                    move.index() != history_model_[current_move_index_].second.index())
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
    applyMove(history_model_[current_move_index_].second.reverse());
}
void Model::onRedo() {
    applyMove(history_model_[current_move_index_ + 1].second);
}
void Model::onUserSelectedHistory(int selected) {
    if (selected == current_move_index_) {
        return;
    }
    int current_move_index = current_move_index_;
    if (selected > current_move_index) {
        // Redo
        for (int i = current_move_index + 1; i <= selected; ++i) {
            applyMove(history_model_[i].second);
        }
    } else {
        // Undo
        for (int i = current_move_index; i >= selected + 1; --i) {
            applyMove(history_model_[i].second.reverse());
        }
    }
}

/* private functions to update */
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
    if (kWinPieceIndex >= static_cast<int>(pieces_.size())) {
        emit canWinStateChanged(false);
    } else {
        emit canWinStateChanged(pieces_[kWinPieceIndex].position() == kWinPosition);
    }
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

/* private function to update step count */
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

/* private function to update current move index */
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

/* check if the pieces is correctly edited */
void Model::validatePieces()
{
    Matrix<int> matrix(kHorizontalUnit, kVerticalUnit);
    for(int i = 0; i < static_cast<int>(pieces_.size()); i++)
    {
        int x, y;
        x = pieces_[i].position().x();
        y = pieces_[i].position().y();
        if(x >= 0 && y >= 0 && y + pieces_[i].size().height() <= kVerticalUnit && x + pieces_[i].size().width() <= kHorizontalUnit)
        {for(int j = x; j <= x + pieces_[i].size().width() - 1; j++){
            for(int k = y; k <= y + pieces_[i].size().height() - 1; k++){
                if(matrix.at(j,k) == 0) {
                    matrix.at(j, k) = 1;
                } else {
                    emit isValidPieces(false);
                    return;
                }
            }
          }
        } else{
            emit isValidPieces(false);
            return;
        }

    }
    emit isValidPieces(true);
}

