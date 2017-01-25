#ifndef PLAYER_MACHINE_LEARNING_AI_H
#define PLAYER_MACHINE_LEARNING_AI_H

#include <QObject>
#include "player_abstract.h"

class PlayerMachineLearningAi : public PlayerAbstract
{
    Q_OBJECT
public:
    explicit PlayerMachineLearningAi(SweeperModel* sweeperModel, QObject* parent = 0);
    void takeNextAction();
};

#endif // PLAYER_MACHINE_LEARNING_AI_H
