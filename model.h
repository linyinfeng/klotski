#ifndef MODEL_H
#define MODEL_H

#include "piece.h"

#include <QObject>
#include <QString>

class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
// Interface
public:
    const static Model default_model;
public:
    const std::vector<Piece> &pieces() const;
    const std::vector<Move> &validMoves() const;
    int stepCount() const;
    int bestStepCount() const;
    const QString &levelName() const;

signals:
    void canWinStateChanged(bool can_win);
    void canUndoStateChanged(bool can_undo);
    void canRedoStateChanged(bool can_redo);

    void syncMove(const Move &move);
    void stepCountChanged(int step_rount);
    void validMovesChanged(const std::vector<Move> &valid_moves);

    void modelLoaded(const Model *model);
    void modelSaved(bool successed);

public slots:
    void applyMove(const Move &move);

    void onUndo();
    void onRedo();

    void onReload();
    void onLoad(const QString & file_name);
    void onSave(const QString & file_name);

private:
    void load(const QString &file_name);

    void updateValidMoves();
    void updateCanWinState();
    void updateCanUndoRedoState();

    void incStepCount();
    void decStepCount();

    void setStepCount(int step_count);

    QString level_name_;
    int best_step_count_;
    std::vector<Piece> pieces_;
    std::vector<Piece> original_pieces_;
    std::vector<Move> valid_moves_;

    int step_count_;

    std::vector<Move> history_;
    int last_move_;
};

#endif // MODEL_H
