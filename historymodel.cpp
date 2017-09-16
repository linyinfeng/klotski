#include <historymodel.h>

#include <QAbstractListModel>
#include <vector>
#include <QDebug>
#include "move.h"

HistoryModel::HistoryModel(QObject *parent) : QAbstractListModel(parent) { }

Move &HistoryModel::operator[](int index) {
    return data_[index];
}

int HistoryModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return data_.size();
}
int HistoryModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::TextAlignmentRole) {
        return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
        QString display(tr("%1 move %2"));
        qDebug() << "List view reqire row" << index.row();
        const Move &move = data_[data_.size() - index.row() - 1];
        QString direction;
        if (move.x() > 0)
            direction = "right";
        else if (move.x() < 0)
            direction = "left";
        else if (move.y() > 0)
            direction = "down";
        else if (move.y() < 0)
            direction = "up";
        else
            direction = "nowhere";
        return display.arg(move.index()).arg(direction);
    }
    return QVariant();
}
Qt::ItemFlags HistoryModel::flags(const QModelIndex &index) const {
    if (index.row() == indexToRow(last_move_)) {
        return Qt::ItemIsEnabled;
    } else {
        return Qt::NoItemFlags;
    }
}

void HistoryModel::clear() {
    beginResetModel();
    data_.clear();
    last_move_ = -1;
    endResetModel();
}
void HistoryModel::resize(int size) {
    beginRemoveColumns(QModelIndex(), indexToRow(size), indexToRow(data_.size() - 1));
    data_.resize(size);
    endRemoveColumns();
}
void HistoryModel::push_back(const Move &move) {
    beginInsertRows(QModelIndex(), indexToRow(data_.size()), indexToRow(data_.size() + 1));
    data_.push_back(move);
    endInsertRows();
}

int HistoryModel::size() const {
    return static_cast<int>(data_.size());
}

int HistoryModel::lastMoveIndex() const {
    return last_move_;
}
Move &HistoryModel::lastMove(int offset) {
    return data_[last_move_ + offset];
}
void HistoryModel::setLastMove(int last_move) {
    beginResetModel();
    last_move_ = last_move;
    endResetModel();
}
void HistoryModel::incLastMove() {
    setLastMove(last_move_ + 1);
}
void HistoryModel::decLastMove() {
    setLastMove(last_move_ - 1);
}

int HistoryModel::indexToRow(int index) const {
    int res = data_.size() - index - 1;
    if (res < 0)
        res = 0;
    return res;
}
