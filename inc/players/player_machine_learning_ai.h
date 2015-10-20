#ifndef PLAYER_MACHINE_LEARNING_AI_H
#define PLAYER_MACHINE_LEARNING_AI_H

#include <QObject>

class PlayerMachineLearningAi : public QObject
{
    Q_OBJECT
public:
    explicit PlayerMachineLearningAi(QObject* parent = 0);
};

#endif // PLAYER_MACHINE_LEARNING_AI_H
