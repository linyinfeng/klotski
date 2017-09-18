#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractItemModel>
#include <QPair>
#include <QObject>
#include <vector>
#include <QDebug>
#include "move.h"

class HistoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    HistoryModel(QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    virtual QModelIndex parent(const QModelIndex &child) const;

    // Interface for operating by model
    void reset();
    void pushBack(int step_count, const Move &move);
    void cutToFit(int size);

    const QPair<int, Move> &operator[](int index) const;

private:
    std::vector<QPair<int, Move>> data_;
};

#endif // HISTORYMODEL_H
