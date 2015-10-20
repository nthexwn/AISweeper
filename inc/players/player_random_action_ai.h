#ifndef PLAYER_RANDOM_ACTION_AI_H
#define PLAYER_RANDOM_ACTION_AI_H

#include <QObject>

class PlayerRandomActionAi : public QObject
{
    Q_OBJECT
public:
    explicit PlayerRandomActionAi(QObject* parent = 0);
};

#endif // PLAYER_RANDOM_ACTION_AI_H
