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
signals:
    void levelNameChanged(const QString &name);
    void fileNameChanged(const QString &file);

    void canWinStateChanged(bool can_win);
    void canUndoStateChanged(bool can_undo);
    void canRedoStateChanged(bool can_redo);

    void syncMove(const Move &move);
    void stepCountChanged(int step_count);
    void bestStepCountChanged(int best_step_count);

    void historyModelChanged(HistoryModel *history_model);
    void currentMoveIndexChanged(int current_move);

    void piecesChanged(const std::vector<Piece> &pieces);
    void validMovesChanged(const std::vector<Move> &valid_moves);

    void savedToFile(bool successed);

public slots:
    /* emit changed for everything */
    void onViewRequireDataRefresh();
    /* move and  sync */
    void applyMove(const Move &move);

    /* undo in model */
    void onUndo();
    /* redo in model */
    void onRedo();

    /* move to history */
    void onUserSelectedHistory(int selected);

    /* reload the game */
    void onReset();

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

    QString file_name_;
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
