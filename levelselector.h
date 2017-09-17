#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <ui_levelselector.h>
#include <QWidget>

class LevelSelector : public QWidget
{
    Q_OBJECT
public:
    explicit LevelSelector(QWidget *parent = nullptr);

signals:
    void loadFile(const QString &file_name);

private:
    void addListItem(const QString &item);

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);

private:
    Ui::levelSelectorView *ui;
};

#endif // FILESELECTOR_H
