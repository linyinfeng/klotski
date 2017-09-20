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
#include <QDesktopServices>
#include <QInputDialog>
#include <QLineEdit>
#include <numeric>

const double View::kFinishButtonVerticalUnit = 0.5;
const double View::kFinishButtonHorizontalUnit = 2;

View::View(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View),
    level_selector(new LevelSelector),
    use_skins_(false),
    edit_mode_(false),
    animation_group_(nullptr),
    step_count_(0),
    best_step_count_(0)

{
    ui->setupUi(this);

    setAcceptDrops(true);
    ui->graphicsView->installEventFilter(this);
    ui->centralWidget->installEventFilter(this);

    // can't be initialized in initialize list
    scene_ = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene_);

    connect(ui->actionSelect_Level, &QAction::triggered, level_selector, &LevelSelector::show);
    connect(level_selector, &LevelSelector::loadFile, this, &View::loadFile);

    connect(ui->actionUndo, &QAction::triggered, this, &View::undo);
    connect(ui->actionRedo, &QAction::triggered, this, &View::redo);
    connect(ui->actionRestart, &QAction::triggered, this, &View::reset);
    connect(ui->pushButtonFinish, &QPushButton::clicked, this, &View::onFinish);

    connect(ui->actionToggle_Skins, &QAction::toggled, this, &View::onToggleSkins);

    connect(ui->actionShow_Statusbar, &QAction::triggered, ui->statusBar,&QToolBar::setVisible);
    connect(ui->actionShow_Toolbar, &QAction::triggered, ui->toolBar, &QToolBar::setVisible);
    connect(ui->actionShow_Dock_Information, &QAction::toggled, ui->dockWidgetGameInfo, &QDockWidget::setVisible);
    connect(ui->dockWidgetGameInfo, &QDockWidget::visibilityChanged, ui->actionShow_Dock_Information, &QAction::setChecked);

    connect(ui->actionOpen, &QAction::triggered, this, &View::promoteToOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &View::promoteToSaveFile);
    connect(ui->actionShow_Optimal_solution, &QAction::triggered, this, &View::onLoadOptimalSolution);

    connect(ui->historyView, &HistoryView::userSelectedHistory, this, &View::userSelectedHistory);

    connect(ui->actionEnglish, &QAction::triggered, this, &View::changeTranslateToEnglish);
    connect(ui->actionChinese_Simplified, &QAction::triggered, this, &View::changeTranslateToChineseSimplified);
    connect(ui->actionAbout_Klotski, &QAction::triggered, this, &View::showAboutDialog);
    connect(ui->actionKlotski_Handbook, &QAction::triggered, this, &View::showHandbook);

    connect(ui->actionQuit, &QAction::triggered, this, &View::close);

    qDebug() << "Resize View at View created";
    resizeView();
}
View::~View()
{
    // Other object will be delete by qt
    delete ui;
    delete level_selector;
}

void View::refresh() {
    qDebug() << "[emit] requireModelDataRefresh()";
    emit requireModelDataRefresh();
}

void View::updateHistoryModel(HistoryModel *history_model) {
    ui->historyView->setModel(history_model);
}
void View::updateCurrentMoveIndex(int index) {
    ui->historyView->updateCurrentMoveIndex(index);
}

void View::updateCanWinStateChanged(bool can_win) {
    ui->pushButtonFinish->setEnabled(can_win);
}
void View::updateCanUndoStateChanged(bool can_undo) {
    ui->actionUndo->setEnabled(can_undo);
}
void View::updateCanRedoStateChanged(bool can_redo) {
    ui->actionRedo->setEnabled(can_redo);
}
void View::updateStepCount(int step_count) {
    step_count_ = step_count;
    updateStepCountInfo();
}
void View::updateBestStepCount(int best_step_count) {
    best_step_count_ = best_step_count;
    updateStepCountInfo();
}

void View::updateStepCountInfo() {
    if (step_count_ <= best_step_count_)
        ui->labelStepInfo->setStyleSheet("color: rgb(0, 170, 0);");
    else {
        ui->labelStepInfo->setStyleSheet("color: rgb(170, 0, 0);");
    }
    ui->labelStepInfo->setText(tr("%1/%2<span style=\"font-size: 11px;\">steps</span>").arg(step_count_).arg(best_step_count_));
}

void View::updatePieces(const std::vector<Piece> &pieces) {
    if (animation_group_) {
        if (animation_group_->state() == QSequentialAnimationGroup::Running ||
                animation_group_->state() == QSequentialAnimationGroup::Paused) {
            animation_group_->stop();
        }
        animation_group_->clear();
    }
    scene_->clear();
    graphics_pieces_.clear();
    int size = static_cast<int>(pieces.size());
    for (int i = 0; i < size; ++i) {
        GraphicsPiece *graphics_piece = new GraphicsPiece(i, pieces[i]);
        graphics_pieces_.push_back(graphics_piece);
        if (use_skins_) {
            graphics_piece->setBackgroundImage(
                        getPieceBackgroundImage(graphics_piece->index(), graphics_piece->piece())
            );
        }
        graphics_piece->setEditMode(edit_mode_);
        connect(graphics_piece, &GraphicsPiece::syncMove, this, &View::syncMove);
        connect(graphics_piece, &GraphicsPiece::addAnimation, this, &View::addSequencedAnimation);
        connect(graphics_piece, &GraphicsPiece::pieceRotated, this, &View::pieceRotated);
        scene_->addItem(graphics_piece);
        graphics_piece->onSceneResize();
    }
    qDebug() << "Level Loaded";
}
QImage View::getPieceBackgroundImage(int index, const Piece &piece) {
    const static QString image_dir(QCoreApplication::applicationDirPath() + "/images/");
    QImage image;
    if (piece.size() == QSize(2, 2)) {
        image.load(image_dir + "2x2.png");
    } else if (piece.size() == QSize(1, 1)) {
        image.load(image_dir + "1x1.png");
    } else if (piece.size() == QSize(1, 2)) {
        image.load(image_dir + QString("1x2/%1.png").arg(index));
    } else if (piece.size() == QSize(2, 1)) {
        image.load(image_dir + QString("2x1/%1.png").arg(index));
    }
    return image;
}

void View::onToggleSkins(bool use_skin) {
    use_skins_ = use_skin;
    if (use_skins_) {
        for (GraphicsPiece *graphics_piece : graphics_pieces_) {
            graphics_piece->setBackgroundImage(getPieceBackgroundImage(graphics_piece->index(), graphics_piece->piece()));
        }
        scene_->setBackgroundBrush(QBrush(QColor(250, 235, 215)));
    } else {
        for (GraphicsPiece *graphics_piece : graphics_pieces_) {
            graphics_piece->setBackgroundImage(QImage());
        }
        scene_->setBackgroundBrush(QBrush(Qt::white));
    }
}

void View::updateValidMoves(const std::vector<Move> &valid_moves) {
    for (GraphicsPiece *graphics_piece : graphics_pieces_) {
        graphics_piece->clearValidMoveDirection();
    }
    for (const Move &valid_move : valid_moves) {
        qDebug() << "addValidMoveDirection" << valid_move.index() << valid_move.x() << valid_move.y();
        graphics_pieces_[valid_move.index()]->addValidMoveDirection(valid_move);
    }
}

void View::updateLevelName(const QString &level_name) {
    level_name_ = level_name;
    this->setWindowTitle(tr("Klotski - %1").arg(level_name_));
    ui->labelLevelName->setText(level_name_);
}
void View::updateFileName(const QString &file_name) {
    qDebug() << "update file name to" << file_name;
    file_name_ = file_name;
}

void View::onSavedToFile(bool successed) {
    if (successed) {
        ui->statusBar->showMessage(tr("Game saved"));
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Failed to save file!"));
    }
}

void View::applyMove(const Move &move) {
    graphics_pieces_[move.index()]->applyMove(move);
}

void View::forceResize() {
    QResizeEvent *event = new QResizeEvent(size(), size());
    QCoreApplication::postEvent(this, event);
}
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
    qDebug() << "ui->graphicsView->fitInView(" << scene_->sceneRect() << ")";
    ui->graphicsView->fitInView(scene_->sceneRect());

    for (GraphicsPiece *graphics_piece : graphics_pieces_) {
//        qDebug() << "Try resize" << graphics_piece;
        graphics_piece->onSceneResize();
    }

    ui->pushButtonFinish->setGeometry(button_rect);
//    qDebug() << "ui->pushButtonFinish->setGeometry" << button_rect;
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
    qDebug() << "[emit] loadFile(event->mimeData()->urls()[0].toLocalFile())";
    emit loadFile(event->mimeData()->urls()[0].toLocalFile());
}

void View::keyPressEvent(QKeyEvent *event) {
    QMainWindow::keyPressEvent(event);
    qDebug() << event->modifiers() << event->key();
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_F11) {
        toggleEditMode();
    }
}

void View::onFinish() {
    qDebug() << "Finish";
    if (step_count_ > best_step_count_)
        QMessageBox::information(this, tr("You win!"),
            tr("Your take %1 steps to finish this level\nThe best solution of this level takes %2 steps")
                .arg(step_count_).arg(best_step_count_));
    else {
        QMessageBox::information(this, tr("You win!"),
            tr("Congratulations!\nYou find the best solution!"));
    }
}
void View::promoteToOpenFile() {
    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("Open Saved game"),
        ".",
        tr("Klotski Save Files (*.%1)").arg(kSaveSuffix));
    qDebug() << "Attempt to open file" << file_name;
    if (!file_name.isEmpty()) {
        QFileInfo file_info(file_name);
        if (file_info.isFile()) {
            qDebug() << "[emit] loadFile(file_name)";
            emit loadFile(file_name);
        } else
            QMessageBox::warning(this, "Warning", tr("\"%1\" can't be find or is not a file").arg(file_name));
    } else
        ui->statusBar->showMessage(tr("No file specified"));
}
void View::promoteToSaveFile() {
    if (!edit_mode_) {
        QString file_name = QFileDialog::getSaveFileName(
            this,
            tr("Open Save file"),
            ".",
            tr("Klotski Save Files (*.%1)").arg(kSaveSuffix));
        qDebug() << "Attempt to save file" << file_name;
        if (!file_name.isEmpty()) {
            qDebug() << "[EMIT] saveToFile(file_name)";
            emit saveToFile(file_name);
        } else {
            ui->statusBar->showMessage(tr("No file specified"));
        }
    } else {
        toggleEditMode(); // auto save
    }
}
void View::showAboutDialog() {
    QMessageBox::about(this, tr("About Klotski"),
        tr("Developed by\n"
           "\tYinfeng Lin\n"
           "\tNianyi Wang\n"
           "\tZuanjie Ma\n"
           "\tYaodanjun Ren\n"
           "\tYutong Deng\n"
           "Github: github.com/0yinf/klotski\n"
           "All picture are from Koei Tecmo Games's\n\tRomance of the Three Kingdoms 13\n"
           "Using Qt in LGPLv3\n"
           "This software should not be used for any commercial purposes"));
}

void View::addSequencedAnimation(QPropertyAnimation *animation) {
    if (animation_group_ == nullptr) {
        animation_group_ = new QSequentialAnimationGroup(this);
        connect(animation_group_, &QSequentialAnimationGroup::finished, this, &View::onAnimationGroupFinished);
    } else if (animation_group_->state() == QSequentialAnimationGroup::Stopped) {
        animation_group_ = new QSequentialAnimationGroup(this);
    } else {
        animation_group_->pause();
        qDebug() << animation_group_->animationCount() << "animation are processing";
    }

    // Fast animation
//    int current_animation_count = animation_group_->animationCount();
//    if (current_animation_count != 0 && current_animation_count != 1) {
//        int new_duration = animation->duration() / current_animation_count;
//        if (new_duration < 50)
//            new_duration = 50;
//        animation->setDuration(new_duration);
//    }
    animation_group_->addAnimation(animation);

    for (GraphicsPiece *piece : graphics_pieces_) {
        piece->animationStarted();
    }

    if (animation_group_->state() == QSequentialAnimationGroup::Stopped) {
        animation_group_->start();
    } else if (animation_group_->state() == QSequentialAnimationGroup::Paused) {
        animation_group_->resume();
    }
}

void View::onAnimationGroupFinished() {
    animation_group_->deleteLater();
    animation_group_ = nullptr;
    for (GraphicsPiece *piece : graphics_pieces_) {
        piece->animationFinished();
    }
    forceResize(); // ensure size is correct
}

void View::onLoadOptimalSolution() {
    QFileInfo file_info(file_name_);
    qDebug() << "load file" << kDefaultSolutionDir + "/" + file_info.fileName();
    loadFile(kDefaultSolutionDir + "/" + file_info.fileName());
    ui->statusBar->showMessage(tr("Optimal solution loaded"));
}

void View::showHandbook() {
    QDesktopServices::openUrl(QUrl(
        QString("file:///") + QCoreApplication::applicationDirPath() + "/help/index.html")
    );
}

void View::toggleEditMode() {
    if (edit_mode_) {
        edit_mode_ = false;
        ui->statusBar->showMessage(tr("Edit mode closed"));
    } else {
        edit_mode_ = true;
        ui->statusBar->showMessage(tr("Edit mode entered"));
    }
    for (GraphicsPiece *graphics_piece : graphics_pieces_) {
        graphics_piece->setEditMode(edit_mode_);
        graphics_piece->clearValidMoveDirection();
    }
    if (!edit_mode_) {
        bool is_ok;
        QString level_name;
        int best_step_count;
        level_name = QInputDialog::getText(this, "Edit Mode",
            "Please enter level name", QLineEdit::Normal, QString(), &is_ok);
        if (!is_ok) {
            toggleEditMode();
            return;
        }
        best_step_count = QInputDialog::getInt(this, "Edit Mode", "Please enter best step count",
            0, 0, std::numeric_limits<int>::max(), 1, &is_ok);
        if (!is_ok) {
            toggleEditMode();
            return;
        }
        emit editModeExited(level_name, best_step_count);
        emit promoteToSaveFile();
    }
}
