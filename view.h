/* Define the game object
 */

#ifndef VIEW_H
#define VIEW_H

#include "model.h"
#include "move.h"
#include "graphicspiece.h"
#include "levelselector.h"

#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>
#include <vector>
#include <QTranslator>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QSequentialAnimationGroup>

namespace Ui {
class View;
}

/* View Object
 * The user interface. display, handle user input and emit signals to model
 */
class View : public QMainWindow
{
    Q_OBJECT
public:
    /* setup model independent data */
    explicit View(QWidget *parent = 0);
    /* release ui */
    ~View();

signals:
    /* emitted when user required undo */
    void undo();
    /* emitted when user required redo */
    void redo();
    /* emitted when user required reload */
    void reset();
    /* emitted when user required load a specific file */
    void loadFile(const QString &file_name);
    /* emitted when user required save game to a specific file */
    void saveToFile(const QString &file_name);
    /* forward history views' signal */
    void userSelectedHistory(int selected);
    /* forward graphics pieces' syncMove signal */
    void syncMove(const Move &move);

    /* require model send data refresh signals */
    void requireModelDataRefresh();

    /* About translate */
    void changeTranslateToEnglish();
    void changeTranslateToChineseSimplified();

    /* edit mode */
    void pieceRotated(int index);
    /* call model clean history and step */
    void editModeExited(const QString &new_level_name, int new_best_step_count);

public slots:
    /* update piece, generate graphics pieces */
    void updatePieces(const std::vector<Piece> &pieces);
    /* forward syncMove signal to specified graphics pieces */
    void applyMove(const Move &move);
    /* forward validMovesChanged signal to spcified graphics pieces */
    void updateValidMoves(const std::vector<Move> &move);

    /* update best step count */
    void updateBestStepCount(int best_step_count);
    /* update step count on status bar */
    void updateStepCount(int step_count); // RENAMED
    /* set the finish button's state */
    void updateCanWinStateChanged(bool can_win);
    /* set the undo action's state */
    void updateCanUndoStateChanged(bool can_undo);
    /* set the redo action's state */
    void updateCanRedoStateChanged(bool can_redo);
    /* update window title */
    void updateLevelName(const QString &level_name);
    /* update level file */
    void updateFileName(const QString &file);

    /* update history model */
    void updateHistoryModel(HistoryModel *history_model);
    /* update list view slection */
    void updateCurrentMoveIndex(int index);
    /* on model saved, promote user if save was successed */
    void onSavedToFile(bool successed);

    /* post resize event to this object */
    void forceResize();

public slots:
    void refresh();

protected:
//    /* resize view and scene */
//    virtual void resizeEvent(QResizeEvent *event) override;
    /* handle graphics view's QEvent::Enter evnet to setting focus when enter */
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    /* drop to open file */
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoadOptimalSolution();
    /* do when user requires open a save */
    void promoteToOpenFile();
    /* do when user requires save */
    void promoteToSaveFile();
    /* About */
    void showAboutDialog();
    /* do when finish button clicked */
    void onFinish();
    /* animation control */
    void addSequencedAnimation(QPropertyAnimation *animation);
    void onAnimationGroupFinished();
    /* set skins */
    void onToggleSkins(bool use_skin);
    /* show handbook */
    void showHandbook();
    /* edit mode */
    void toggleEditMode();
    /* about settings */
    void saveSettings();
    void loadSettings();

private:
    /* the main resize function */
    void resizeView();
    /* update step count info label */
    void updateStepCountInfo();
    /* get piece's brush */
    QImage getPieceBackgroundImage(int index, const Piece &piece);
    /* cancel all animation */
    void cancelAllAnimation();

    /* define the unit button takes */
    static const double kFinishButtonVerticalUnit;
    static const double kFinishButtonHorizontalUnit;

private:
    /* UI */
    Ui::View *ui;
    LevelSelector *level_selector;

    QString current_language_;

    QGraphicsScene *scene_;
    std::vector<GraphicsPiece *> graphics_pieces_;

    bool use_skins_;
    QImage two_by_two_images_;
    std::vector<QImage> one_by_two_images_;
    std::vector<QImage> two_by_one_images_;
    QImage one_by_one_images_;

    bool edit_mode_;

    QSequentialAnimationGroup *animation_group_;

    QString level_name_;
    QString file_name_;
    int step_count_, best_step_count_;
};

#endif // VIEW_H
