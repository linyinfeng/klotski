#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gameview.h"
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void connectSignalSlot();

    virtual void resizeEvent(QResizeEvent *event) override;

    static const double FinishButtonVerticalUnit;
    static const double FinishButtonHorizontalUnit;
    static const double MainAreaHeightWidthRatio;
    static const double ViewButtonHeightRatio;
    static const double ViewButtonWidthtRatio;

    Ui::MainWindow *ui;
    QLabel *step_info;

    QPushButton *finish_button;

    GameView *game_view;
    GameModel *game_model;

signals:
    void gameViewResized();

private slots:
    void onCentralWidgetResized();
};

#endif // MAINWINDOW_H
