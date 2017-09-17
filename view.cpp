#include "common.h"
#include "view.h"
#include "ui_view.h"

#include <QDebug>
#include <QBrush>
#include <QDialog>
#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QFileInfo>
#include <QFontMetrics>
#include <QMimeData>
#include <QList>
#include <QUrl>

const double View::kFinishButtonVerticalUnit = 0.5;
const double View::kFinishButtonHorizontalUnit = 2;

View::View(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View)
{
    ui->setupUi(this);

    level_selector = new LevelSelector;
    // Only show when user take action
    connect(ui->actionSelect_Level, SIGNAL(triggered(bool)), level_selector, SLOT(show()));

    setAcceptDrops(true);
    ui->graphicsView->setAcceptDrops(true);

//    this->setStyleSheet("* { font-size: 13px; }");
    ui->statusBar->setStyleSheet("#statusBar { border-top: 1px solid rgb(220, 220, 220); } * { font-weight:bold; }");

    ui->centralWidget->installEventFilter(this);

    scene_ = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->installEventFilter(this);
//    ui->graphicsView->setBackgroundBrush(QBrush(Qt::white));

    connect(level_selector, &LevelSelector::loadFile, this, &View::loadFile);

    connect(ui->actionUndo, &QAction::triggered, this, &View::undo);
    connect(ui->actionRedo, &QAction::triggered, this, &View::redo);
    connect(ui->actionRestart, &QAction::triggered, this, &View::reload);
    connect(ui->pushButtonFinish, &QPushButton::clicked, this, &View::onFinish);

    connect(ui->actionShow_Statusbar, &QAction::triggered, ui->statusBar,&QToolBar::setVisible);
    connect(ui->actionShow_Toolbar, &QAction::triggered, ui->toolBar, &QToolBar::setVisible);

    connect(ui->actionOpen, &QAction::triggered, this, &View::onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &View::onSaveFile);

    connect(ui->historyView, &HistoryView::userSelectedHistory, this, &View::userSelectedHistory);

    connect(ui->actionEnglish, &QAction::triggered, this, &View::changeTranslateToEnglish);
    connect(ui->actionChinese_Simplified, &QAction::triggered, this, &View::changeTranslateToChineseSimplified);
    connect(ui->actionAbout_Klotski, &QAction::triggered, this, &View::showAbout);
}
View::~View()
{
    delete ui;
}

void View::setModel(Model *model) {
    model_ = model;
    HistoryModel *history_model = model->historyModel();
    ui->historyView->setModel(history_model);
}

void View::updateStepCount(int step_count) {
    if (step_count <= model_->bestStepCount())
        ui->labelStepInfo->setStyleSheet("color: rgb(0, 170, 0);");
    else {
        ui->labelStepInfo->setStyleSheet("color: rgb(170, 0, 0);");
    }
    ui->labelStepInfo->setText(tr("%1/%2<span style=\"font-size: 11px;\">steps</span>").arg(step_count).arg(model_->bestStepCount()));
    qDebug() << "Step Info label changed" << ui->labelStepInfo->text();
}
void View::updateCurrentMoveIndex(int index) {
    ui->historyView->updateCurrentMoveIndex(index);
}
void View::onCanWinStateChanged(bool can_win) {
    ui->pushButtonFinish->setEnabled(can_win);
}
void View::onCanUndoStateChanged(bool can_undo) {
    ui->actionUndo->setEnabled(can_undo);
}
void View::onCanRedoStateChanged(bool can_redo) {
    ui->actionRedo->setEnabled(can_redo);
}

void View::onModelLoaded(const Model *model) {
    qDebug() << "View::onModelLoaded" << model;

    scene_->clear();
    graphics_pieces_.clear();
    const std::vector<Piece> &pieces = model->pieces();
    int size = pieces.size();
    for (int i = 0; i < size; ++i) {
        GraphicsPiece *graphics_piece = new GraphicsPiece(i, pieces[i]);
        graphics_pieces_.push_back(graphics_piece);
        connect(graphics_piece, SIGNAL(syncMove(Move)), this, SIGNAL(syncMove(Move)));
        scene_->addItem(graphics_piece);
        graphics_piece->onSceneResize();
    }
//    updateValidMoves();

    this->setWindowTitle(tr("Klotski - %1 - %2").arg(model_->levelName()).arg(model_->bestStepCount()));
    ui->statusBar->showMessage(tr("Level Loaded"));

    updateStepCount(model_->stepCount());
}
void View::onModelSaved(bool successed) {
    if (successed)
        ui->statusBar->showMessage(tr("Game saved"));
    else
        QMessageBox::warning(this, tr("Warning"), tr("Failed to open file"));
}

void View::applyMove(const Move &move) {
    graphics_pieces_[move.index()]->applyMove(move);
}
void View::updateValidMoves() {
    onValidMovesChanged(model_->validMoves());
}
void View::onValidMovesChanged(const std::vector<Move> &valid_moves) {
    if (model_ != nullptr) {
        for (GraphicsPiece *graphics_piece : graphics_pieces_) {
            graphics_piece->clearValidMoveDirection();
        }
        for (const Move &valid_move : valid_moves) {
            qDebug() << "addValidMoveDirection" << valid_move.index() << valid_move.x() << valid_move.y();
            graphics_pieces_[valid_move.index()]->addValidMoveDirection(valid_move);
        }
    }
}

void View::forceResize() {
    QResizeEvent *event = new QResizeEvent(size(), size());
    QCoreApplication::postEvent(this, event);
}

//void View::resizeEvent(QResizeEvent *event) {
//    QMainWindow::resizeEvent(event);
void View::resizeView() {
    const double MainAreaHeightWidthRatio =
        (kVerticalUnit + kFinishButtonVerticalUnit) / kHorizontalUnit;
    const double ViewButtonHeightRatio =
        kVerticalUnit / kFinishButtonVerticalUnit;
    const double ViewButtonWidthtRatio =
        kFinishButtonHorizontalUnit / kHorizontalUnit;

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

    ui->graphicsView->setGeometry(view_rect);
    qDebug() << "ui->graphicsView->setGeometry" << view_rect;
    scene_->setSceneRect(QRectF(QPointF(0, 0), view_rect.size()));
    ui->graphicsView->fitInView(scene_->sceneRect());

    for (GraphicsPiece *graphics_piece : graphics_pieces_) {
        qDebug() << "Try resize" << graphics_piece;
        graphics_piece->onSceneResize();
    }

    ui->pushButtonFinish->setGeometry(button_rect);
    qDebug() << "ui->pushButtonFinish->setGeometry" << button_rect;
}
bool View::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->graphicsView) {
        if (event->type() == QEvent::Enter) {
            ui->graphicsView->setFocus(Qt::MouseFocusReason);
            qDebug() << "GraphicsView focus set";
            return true;
        } else if (event->type() == QEvent::Leave) {
            ui->graphicsView->clearFocus();
            qDebug() << "GraphicsView focus clear";
            return true;
        } else {
            return false;
        }
    } else if (watched == ui->centralWidget) {
        if (event->type() == QEvent::Resize) {
            resizeView();
            return true;
        } else {
            return false;
        }
    } else {
        return QMainWindow::eventFilter(watched, event);
    }
}
void View::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *mime_data = event->mimeData();
    qDebug() << mime_data;
    if (mime_data) {
        QList<QUrl> urls = mime_data->urls();
        qDebug() << urls;
        if (urls.length() > 0) {
            QUrl url = urls[0];
            qDebug() << url;
            QFileInfo info(url.toLocalFile());
            if (info.suffix() == kSaveSuffix) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}
void View::dropEvent(QDropEvent *event) {
    emit loadFile(event->mimeData()->urls()[0].toLocalFile());
}

void View::onFinish() {
    qDebug() << "Finish";
    if (model_->stepCount() > model_->bestStepCount())
        QMessageBox::information(this, tr("You win!"),
            tr("Your take %1 steps to finish this level\nThe best solution of this level takes %2 steps")
                .arg(model_->stepCount()).arg(model_->bestStepCount()));
    else {
        QMessageBox::information(this, tr("You win!"),
            tr("Congratulations!\nYou find the best solution!"));
    }
}

void View::onOpenFile() {
    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("Open Saved game"),
        ".",
        tr("Klotski Save Files (*.%1)").arg(kSaveSuffix));
    qDebug() << "Attempt to open file" << file_name;
    if (!file_name.isEmpty()) {
        QFileInfo file_info(file_name);
        if (file_info.isFile())
            emit loadFile(file_name);
        else
            QMessageBox::warning(this, "Warning", tr("\"%1\" can't be find or is not a file ").arg(file_name));
    } else
        ui->statusBar->showMessage(tr("No file specified"));
}
void View::onSaveFile() {
    QString file_name = QFileDialog::getSaveFileName(
        this,
        tr("Open Save file"),
        ".",
        tr("Klotski Save Files (*.%1)").arg(kSaveSuffix));
    qDebug() << "Attempt to save file" << file_name;
    if (!file_name.isEmpty())
        emit saveToFile(file_name);
    else
        ui->statusBar->showMessage(tr("No file specified"));
}

void View::showAbout() {
    QMessageBox::about(this, tr("About Klotski"),
        tr("Developed by\n"
           "\tYinfeng Lin\n"
           "\tNianyi Wang\n"
           "\tZhuanjie Ma\n"
           "\tYaodanjun Ren\n"
           "\tYutong Deng\n"
           "Github: github.com/0yinf/klotski\n"
           "Using Qt in LGPLv3"));
}
