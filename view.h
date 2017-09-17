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
    void reload();
    /* emitted when user required load a specific file */
    void loadFile(const QString &file_name);
    /* emitted when user required save game to a specific file */
    void saveToFile(const QString &file_name);
    /* forward history views' signal */
    void userSelectedHistory(int selected);
    /* send to Game Class to reload hold view */
    void viewReload();
    /* forward graphics pieces' syncMove signal */
    void syncMove(const Move &move);

    /* About translate */
    void changeTranslateToEnglish();
    void changeTranslateToChineseSimplified();

public slots:
    /* forward syncMove signal to specified graphics pieces */
    void applyMove(const Move &move);
    /* forward validMovesChanged signal to spcified graphics pieces
     */
    void onValidMovesChanged(const std::vector<Move> &move);

    /* update step count on status bar */
    void updateStepCount(int step_count); // RENAMED
    /* set the finish button's state */
    void onCanWinStateChanged(bool can_win);
    /* set the undo action's state */
    void onCanUndoStateChanged(bool can_undo);
    /* set the redo action's state */
    void onCanRedoStateChanged(bool can_redo);
    /* do when finish button clicked */
    void onFinish();

    /* update list view slection */
    void updateCurrentMoveIndex(int index);
    /* on model saved
     * promote user if save was successed
     */

    void onModelSaved(bool successed);
    /* on model loaded
     * reload every data about model(mostly graphics pieces)
     */
    void onModelLoaded(const Model *model);


    /* post resize event to this object */
    void forceResize();


public slots:
    void setModel(Model *);

protected:
//    /* resize view and scene */
//    virtual void resizeEvent(QResizeEvent *event) override;
    /* handle graphics view's QEvent::Enter evnet to setting focus when enter */
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    /* drop to open file */
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

private slots:
    /* do when user requires open a save */
    void onOpenFile();
    /* do when user requires save */
    void onSaveFile();
    /* About */
    void showAbout();

private:
    /* the main resize function */
    void resizeView();

    /* force update valid moves
     */
    void updateValidMoves();

    /* define the unit button takes */
    static const double kFinishButtonVerticalUnit;
    static const double kFinishButtonHorizontalUnit;

private:
    /* UI */
    Ui::View *ui;

    QGraphicsScene *scene_;
    std::vector<GraphicsPiece *> graphics_pieces_;

    QTranslator translator;

    LevelSelector *level_selector;

private:
    Model *model_;
};

#endif // VIEW_H
