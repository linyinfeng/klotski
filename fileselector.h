#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <ui_levelselector.h>
#include <QWidget>

class fileselector : public QWidget
{
    Q_OBJECT
public:
    explicit fileselector(QWidget *parent = nullptr);

signals:

public slots:

private:
    Ui::levelSelectorView *ui;
};

#endif // FILESELECTOR_H
