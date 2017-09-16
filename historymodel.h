#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractListModel>
#include <vector>
#include <QDebug>
#include "move.h"

class HistoryModel : public QAbstractListModel
{
public:
    HistoryModel(QObject *parent = 0);

    Move &operator[](int index);

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    void clear();
    void resize(int size);
    void push_back(const Move &move);

    int size() const;

    int lastMoveIndex() const;
    Move &lastMove(int offset = 0);
    void setLastMove(int last_move);
    void incLastMove();
    void decLastMove();

private:
    int indexToRow(int index) const;
    std::vector<Move> data_;
    int last_move_;
};

#endif // HISTORYMODEL_H
