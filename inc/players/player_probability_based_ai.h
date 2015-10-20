#ifndef PLAYER_PROBABILITY_BASED_AI_H
#define PLAYER_PROBABILITY_BASED_AI_H

#include <QObject>

class PlayerProbabilityBasedAi : public QObject
{
    Q_OBJECT
public:
    explicit PlayerProbabilityBasedAi(QObject* parent = 0);
};

#endif // PLAYER_PROBABILITY_BASED_AI_H
