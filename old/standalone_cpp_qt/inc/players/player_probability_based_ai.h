#ifndef PLAYER_PROBABILITY_BASED_AI_H
#define PLAYER_PROBABILITY_BASED_AI_H

#include <QObject>
#include "player_abstract.h"

class PlayerProbabilityBasedAi : public PlayerAbstract
{
    Q_OBJECT
public:
    explicit PlayerProbabilityBasedAi(SweeperModel* sweeperModel, QObject* parent = 0);
    void takeNextAction();
};

#endif // PLAYER_PROBABILITY_BASED_AI_H
