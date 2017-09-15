/* Define the game object
 */

#ifndef VIEW_H
#define VIEW_H

#include "model.h"
#include "move.h"
#include "graphicspiece.h"

#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>
#include <vector>

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
    void load(const QString &file_name);
    /* emitted when user required save game to a specific file */
    void save(const QString &file_name);

public slots:
    /* update step count on status bar */
    void updateStepCount(int step_count); // RENAMED
    /* set the finish button's state */
    void onCanWinStateChanged(bool can_win);
    /* set the undo action's state */
    void onCanUndoStateChanged(bool can_undo);
    /* set the redo action's state */
    void onCanRedoStateChanged(bool can_redo);

    /* on model saved
     * promote user if save was successed
     */
    void onModelSaved(bool successed);

public slots:
    /* on model loaded
     * reload every data about model(mostly graphics pieces)
     */
    void onModelLoaded(const Model *model);

signals:
    /* forward graphics pieces' syncMove signal */
    void syncMove(const Move &move);
public slots:
    /* forward syncMove signal to specified graphics pieces
     */
    void applyMove(const Move &move);
    /* forward validMovesChanged signal to spcified graphics pieces
     */
    void onValidMovesChanged(const std::vector<Move> &move);
private:
    /* force update valid moves
     */
    void updateValidMoves();

public slots:
    /* post resize event to this object */
    void forceResize();
protected:
    /* resize view and scene */
    virtual void resizeEvent(QResizeEvent *event) override;
    /* handle graphics view's QEvent::Enter evnet to setting focus when enter */
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    /* define the unit button takes */
    static const double kFinishButtonVerticalUnit;
    static const double kFinishButtonHorizontalUnit;

private slots:
    /* do when finish button clicked */
    void onFinish();

    /* do when user requires open a save */
    void onOpenFile();
    /* do when user requires save */
    void onSaveFile();

private:
    /* UI */
    Ui::View *ui;
    QLabel *step_info_;
    QGraphicsScene *scene_;

    std::vector<GraphicsPiece *> graphics_pieces_;

private:
    const Model *model_;
};

#endif // VIEW_H
