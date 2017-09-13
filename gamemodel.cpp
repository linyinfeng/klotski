#include "gamemodel.h"
#include <QObject>
#include "gamecommon.h"


GameModel::GameModel(QObject *parent) : QObject(parent)
{

}

GameModel::~GameModel() {

}

void GameModel::onMove(const game::Move &move)
{

}

void GameModel::onUndo()
{

}

void GameModel::onRedo()
{

}
