#include <historymodel.h>

#include <vector>
#include <QDebug>
#include <algorithm>
#include "move.h"

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractItemModel(parent) {
}

int HistoryModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return data_.size();
}
int HistoryModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}
QModelIndex HistoryModel::index(int row, int column, const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }
    switch (section) {
    case 0:
        return tr("Piece - Move");
    default:
        return QVariant();
    }
}
QVariant HistoryModel::data(const QModelIndex &index, int role) const {
//    if (role != Qt::DisplayRole || !index.isValid())
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (index.column()) {
    case 0: // #
    {
        QString move_direction;
        const Move &move = data_[index.row()];
        if (move.x() == 1)
            move_direction = tr("Right");
        else if (move.x() == -1)
            move_direction = tr("Left");
        else if (move.y() == 1)
            move_direction = tr("Down");
        else if (move.y() == -1)
            move_direction = tr("Up");
        return tr("#%1 - %2").arg(data_[index.row()].index()).arg(move_direction);
    }
    default:
        return QVariant();
    }
}

bool HistoryModel::insertRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), row, row + count - 1);
    std::vector<Move>::iterator last = data_.begin();
    std::advance(last, row);
    data_.insert(last, count, Move(-1, 0, 0));
    endInsertRows();
    return true;
}
bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if (count == 0) return false;
    beginRemoveRows(QModelIndex(), row, row + count -1);
    std::vector<Move>::iterator first = data_.begin(), last;
    std::advance(first, row);
    last = first;
    std::advance(last, count);
    data_.erase(first, last);
    endRemoveRows();
    return true;
}

bool HistoryModel::setData(const QModelIndex &index, const QVariant &value, int role) {
//    if (!index.isValid() || role != Qt::EditRole)
    if (role != Qt::EditRole)
        return false;
    const Move *move = value.value<const Move *>();
    data_[index.row()] = *move;
    emit dataChanged(createIndex(index.row(), 0), createIndex(index.row(), columnCount() - 1));
    return true;
}

QModelIndex HistoryModel::parent(const QModelIndex &child) const {
    Q_UNUSED(child);
    return QModelIndex();
}

void HistoryModel::reset() {
    beginResetModel();
    data_.clear();
    endResetModel();
}

void HistoryModel::pushBack(const Move &move) {
    insertRow(rowCount());
    qDebug() << "setData at" << createIndex(rowCount() - 1, 0);
    QVariant variant;
    variant.setValue(&move);
    setData(createIndex(rowCount() - 1, 0), variant);
}

void HistoryModel::cutToFit(int size) {
    removeRows(size, rowCount() - size);
}

const Move &HistoryModel::operator[](int index) const {
    return data_[index];
}
