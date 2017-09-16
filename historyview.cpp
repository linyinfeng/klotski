#include "historyview.h"

HistoryView::HistoryView(QWidget *parent) : QListView(parent)
{
}

void HistoryView::updateCurrentMoveIndex(int index) {
    setCurrentIndex(model()->index(index, 0));
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
            emit userSelectedHistory(aim);
        }
    }
}
