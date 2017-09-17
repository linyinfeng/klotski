#include "historyview.h"
#include <QListView>
#include <QDebug>

HistoryView::HistoryView(QWidget *parent) : QListView(parent)
{
//    setSelectionBehavior(QAbstractItemView::SelectRows);
//    setSelectionMode(QAbstractItemView::SingleSelection);
}

void HistoryView::updateCurrentMoveIndex(int index) {
    if (index != -1)
        setCurrentIndex(model()->index(index, 0));
    else
        clearSelection();
}

void HistoryView::rowsInserted(const QModelIndex &parent, int start, int end) {
    QListView::rowsInserted(parent, start, end);
    scrollToBottom();
}

void HistoryView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    // use first index

    Q_UNUSED(deselected);
    const QList<QModelIndex> &selected_indexs = selected.indexes();
    if (selected_indexs.size() > 0) {
        QModelIndex index = selected_indexs[0];
        if (index.isValid()) {
            int aim = index.row();
            qDebug() << "[EMIT] duserSelectedHistory(aim)";
            emit userSelectedHistory(aim);
        }
    }
}
