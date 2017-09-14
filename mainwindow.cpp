#include "common.h"
#include "ui_mainwindow.h"

#include <QtGui>
#include <QDebug>
#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QList>

extern const double MainWindow::FinishButtonVerticalUnit = 0.5;
extern const double MainWindow::FinishButtonHorizontalUnit = 2;
extern const double MainWindow::DefaultHorizontalUnit = 4;
extern const double MainWindow::DefaultVerticalUnit = 5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup main window
    ui->setupUi(this);
    setWindowTitle(tr("Klotski"));

    // Setup status bar
    ui->statusBar->setStyleSheet("#statusBar { border-top: 2px dashed rgb(220, 220, 220); }");
    stepInfo = new QLabel(ui->statusBar);
    stepInfo->setObjectName("step_info");
    ui->statusBar->addPermanentWidget(stepInfo);
    stepInfo->setText("0 steps");
    stepInfo->setStyleSheet("#step_info { font-size: 15px; } ");

    // Setup finish_button
    finishButton = new QPushButton(ui->centralWidget);
    finishButton->setObjectName("finish_button");
    finishButton->setText("Finish");

    setupView();

    connectSignalSlot();
}

void MainWindow::setupView() {

}

void MainWindow::setModel(const game::Model *model) {
    m_model = model;
}


void MainWindow::connectSignalSlot()
{
    connect(ui->actionShow_Statusbar, &QAction::toggled, ui->statusBar, &QStatusBar::setVisible);
    connect(ui->actionShow_Toolbar, &QAction::toggled, ui->toolBar, &QToolBar::setVisible);
    connect(ui->actionShow_Statusbar, &QAction::changed, this, &MainWindow::forceResize);
    connect(ui->actionShow_Toolbar, &QAction::changed, this, &MainWindow::forceResize);
}

void MainWindow::forceResize()
{
    QResizeEvent *event = new QResizeEvent(this->size(), this->size());
    QCoreApplication::postEvent(this, event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    const double VerticalUint = m_model ?  m_model->verticalUint() : DefaultVerticalUnit;
    const double HorizontalUnit =  m_model ?  m_model->horizontalUnit() : DefaultHorizontalUnit;

    const double MainAreaHeightWidthRatio =
        (VerticalUint + FinishButtonVerticalUnit) / HorizontalUnit;
    const double ViewButtonHeightRatio =
        VerticalUint / FinishButtonVerticalUnit;
    const double ViewButtonWidthtRatio =
        FinishButtonHorizontalUnit / HorizontalUnit;

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
    button_rect.setWidth(main_area_rect.width() * ViewButtonWidthtRatio);
    button_rect.setHeight(main_area_rect.height() - view_rect.height());
    button_rect.moveLeft(cw_width / 2 - button_rect.width() / 2);
    button_rect.moveTop(main_area_rect.y() + view_rect.height());

    m_view->setGeometry(view_rect);
    finishButton->setGeometry(button_rect);

    qDebug() << ui->centralWidget << ui->centralWidget->geometry();
    qDebug() << m_view << m_view->geometry();
    qDebug() << finishButton << finishButton->geometry();
}


