#ifndef MODEL_H
#define MODEL_H

#include "piece.h"

#include <QObject>
#include <QString>
#include <historymodel.h>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
// Interface
public:
    const static Model default_model;
public:
    /* get the state of the pieces */
    const std::vector<Piece> &pieces() const;
    /* get the valid moves of the pieces */
    const std::vector<Move> &validMoves() const;
    /* get the step count */
    int stepCount() const;
    /* get the best step count of the level */
    int bestStepCount() const;
    /* get level name */
    const QString &levelName() const;
    /* get history model */
    HistoryModel *historyModel();
    /* get current move index */
    int currentMoveIndex();

signals:
    void canWinStateChanged(bool can_win);
    void canUndoStateChanged(bool can_undo);
    void canRedoStateChanged(bool can_redo);

    void syncMove(const Move &move);
    void stepCountChanged(int step_rount);
    void currentMoveIndexChanged(int current_move);
    void validMovesChanged(const std::vector<Move> &valid_moves);

    void modelLoaded(const Model *model);
    void modelSaved(bool successed);

public slots:
    /* move and  sync */
    void applyMove(const Move &move);

    /* undo in model */
    void onUndo();
    /* redo in model */
    void onRedo();

    /* move to history */
    void onUserSelectedHistory(int selected);

    /* reload the game */
    void onReload();

    /* do when user reqires load */
    void onLoadFile(const QString & file_name);
    /* save to file */
    void onSaveToFile(const QString & file_name);

private:
    /**/
    void load(const QString &file_name);

    void updateValidMoves();
    void updateCanWinState();
    void updateCanUndoRedoState();

    void incStepCount();
    void decStepCount();
    void setStepCount(int step_count);

    void incCurrentMoveIndex();
    void decCurrentMoveIndex();
    void setCurrentMoveIndex(int current_move);

    QString level_name_;
    int best_step_count_;
    std::vector<Piece> pieces_;
    std::vector<Piece> original_pieces_;
    std::vector<Move> valid_moves_;

    int step_count_;

    HistoryModel history_model_;
    int current_move_index_;
};

#endif // MODEL_H
