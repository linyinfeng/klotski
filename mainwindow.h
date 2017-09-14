#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game_model.h"
#include "game_view.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void setModel(const game::Model *model);

private:
    void connectSignalSlot();

    virtual void resizeEvent(QResizeEvent *event) override;

    static const double FinishButtonVerticalUnit;
    static const double FinishButtonHorizontalUnit;
    static  const double DefaultHorizontalUnit;
    static const double DefaultVerticalUnit;

    void setupView();

    // UI
    Ui::MainWindow *ui;
    QLabel *stepInfo;
    QPushButton *finishButton;
    game::View *m_view;

    // Model
    const game::Model *m_model;

    void gameViewResized();

private slots:
    void forceResize();
};

#endif // MAINWINDOW_H
