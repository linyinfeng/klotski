#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameview.h"
#include "gamecommon.h"
#include <QtGui>
#include <QDebug>

extern const double MainWindow::FinishButtonVerticalUnit = 0.5;
extern const double MainWindow::FinishButtonHorizontalUnit = 0.5;
extern const double MainWindow::MainAreaHeightWidthRatio =
    (VerticalUnit + FinishButtonVerticalUnit) / HorizontalUnit;
extern const double MainWindow::ViewButtonHeightRatio =
    VerticalUnit / FinishButtonVerticalUnit;
extern const double MainWindow::ViewButtonWidthtRatio =
    HorizontalUnit / FinishButtonHorizontalUnit;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup main window
    ui->setupUi(this);
    setWindowTitle(tr("Klotski"));

    // Setup status bar

    ui->statusBar->setStyleSheet("#statusBar { border-top: 2px dashed rgb(220, 220, 220); }");
    step_info = new QLabel(ui->statusBar);
    step_info->setObjectName("step_info");
    ui->statusBar->addPermanentWidget(step_info);
    step_info->setText("0 steps");
    step_info->setStyleSheet("#step_info { font-size: 15px; } ");

    // Setup finish_button
    finish_button = new QPushButton(ui->centralWidget);
    finish_button->setObjectName("finish_button");
    finish_button->setText("Finish");

    // Setup game_view
    game_view = new GameView(ui->centralWidget);
    game_view->setObjectName("game_view");

    // Setup game_model
    game_model = new GameModel();
    connectSignalSlot();
}

void MainWindow::connectSignalSlot()
{
    connect(ui->actionShow_Statusbar, &QAction::toggled, ui->statusBar, &QStatusBar::setVisible);
    connect(ui->actionShow_Toolbar, &QAction::toggled, ui->toolBar, &QToolBar::setVisible);
    connect(ui->actionShow_Statusbar, &QAction::changed, this, &MainWindow::onCentralWidgetResized);
    connect(ui->actionShow_Toolbar, &QAction::changed, this, &MainWindow::onCentralWidgetResized);

    connect(this, &MainWindow::gameViewResized, game_view, &GameView::onGameViewResized);
}

void MainWindow::onCentralWidgetResized()
{
    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    QCoreApplication::postEvent(this, event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    QSize old_game_view_size = game_view->size();

    int cw_height = ui->centralWidget->height();
    int cw_width  = ui->centralWidget->width();

    QRect main_area_rect;
    if ((static_cast<double>(cw_height) / cw_width) > MainAreaHeightWidthRatio) {
        // space top and bottom. area base on width
        main_area_rect.setWidth(cw_width);
        main_area_rect.setHeight(cw_width * MainAreaHeightWidthRatio);
    } else {
        // space left and right. area base on height
        main_area_rect.setWidth(cw_height / MainAreaHeightWidthRatio);
        main_area_rect.setHeight(cw_height);
    }
    main_area_rect.moveLeft(cw_width / 2 - main_area_rect.width() / 2);
    main_area_rect.moveTop(cw_height / 2 - main_area_rect.height() / 2);

    QRect view_rect, button_rect;
    view_rect.setTopLeft(main_area_rect.topLeft());
    view_rect.setSize(QSize(main_area_rect.width(),
        main_area_rect.height() / (ViewButtonHeightRatio + 1) * ViewButtonHeightRatio));
    button_rect.setWidth(main_area_rect.width() / 2);
    button_rect.setHeight(main_area_rect.height() - view_rect.height());
    button_rect.moveLeft(cw_width / 2 - button_rect.width() / 2);
    button_rect.moveTop(main_area_rect.y() + view_rect.height());

    game_view->setGeometry(view_rect);
    finish_button->setGeometry(button_rect);

    qDebug() << "Resized...";
    qDebug() << ui->centralWidget << ui->centralWidget->geometry();
    qDebug() << game_view << game_view->geometry();
    qDebug() << finish_button << finish_button->geometry();

    qDebug() << "Old game view size" << old_game_view_size;
    qDebug() << "New game view size" << game_view->size();
    if (game_view->size() != old_game_view_size)
        emit gameViewResized();
}


