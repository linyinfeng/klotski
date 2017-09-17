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
//    case 0:
//        return tr("Step");
//    case 1:
//        return tr("Piece");
//    case 2:
//        return tr("Move");
    case 0:
        return tr("Step-Piece-Move");
    default:
        return QVariant();
    }
}
QVariant HistoryModel::data(const QModelIndex &index, int role) const {
//    if (role != Qt::DisplayRole || !index.isValid())
    if (role != Qt::DisplayRole)
        return QVariant();
    QPair<int, Move> pair = data_[index.row()];

    QString direction;
    switch (index.column()) {
    case 0:
//        return pair.first;
//        return pair.second.index();
        if (pair.second.x() == 1)
            direction = tr("Right");
        else if (pair.second.x() == -1)
            direction = tr("Left");
        else if (pair.second.y() == 1)
            direction = tr("Down");
        else if (pair.second.y() == -1)
            direction = tr("Up");
        else
            direction = tr("Error");
        return tr("Step %1 - #%2 %3").arg(pair.first).arg(pair.second.index()).arg(direction);
    default:
        return QVariant();
    }
}

bool HistoryModel::insertRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), row, row + count - 1);
    std::vector<QPair<int, Move>>::iterator last = data_.begin();
    std::advance(last, row);
    data_.insert(last, count, QPair<int, Move>(0, Move(-1, 0, 0)));
    endInsertRows();
    return true;
}
bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if (count == 0) return false;
    beginRemoveRows(QModelIndex(), row, row + count -1);
    std::vector<QPair<int, Move>>::iterator first = data_.begin(), last;
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
    QPair<int, const Move *> pair = value.value<QPair<int, const Move *>>();
    data_[index.row()].second = *pair.second;
    data_[index.row()].first = pair.first;
    qDebug() << "[EMIT] dataChanged(createIndex(index.row(), 0), createIndex(index.row(), columnCount() - 1))";
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

void HistoryModel::pushBack(int step_count, const Move &move) {
    insertRow(rowCount());
    qDebug() << "setData at" << createIndex(rowCount() - 1, 0);
    QVariant variant;
    variant.setValue(QPair<int, const Move *>(step_count, &move));
    setData(createIndex(rowCount() - 1, 0), variant);
}

void HistoryModel::cutToFit(int size) {
    removeRows(size, rowCount() - size);
}

const Move &HistoryModel::operator[](int index) const {
    return data_[index].second;
}
