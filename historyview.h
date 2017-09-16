#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QListView>

class HistoryView : public QListView
{
    Q_OBJECT
public:
    HistoryView(QWidget *parent = nullptr);

signals:
    void userSelectedHistory(int selected);

public slots:
    void updateCurrentMoveIndex(int index);

protected:
    virtual void rowsInserted(const QModelIndex &parent, int start, int end) override;
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
};

#endif // HISTORYVIEW_H
