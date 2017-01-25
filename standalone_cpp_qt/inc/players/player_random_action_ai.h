#ifndef PLAYER_RANDOM_ACTION_AI_H
#define PLAYER_RANDOM_ACTION_AI_H

#include <QObject>
#include "player_abstract.h"

class PlayerRandomActionAi : public PlayerAbstract
{
    Q_OBJECT
public:
    explicit PlayerRandomActionAi(SweeperModel* sweeperModel, QObject* parent = 0);
    void takeNextAction();
};

#endif // PLAYER_RANDOM_ACTION_AI_H
