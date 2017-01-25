#ifndef PLAYER_HUMAN_H
#define PLAYER_HUMAN_H

#include <QObject>
#include "player_abstract.h"

class PlayerHuman : public PlayerAbstract
{
    Q_OBJECT
public:
    explicit PlayerHuman(SweeperModel* sweeperModel, QObject* parent = 0);
    void takeNextAction();
};

#endif // PLAYER_HUMAN_H
