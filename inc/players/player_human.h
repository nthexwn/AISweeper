#ifndef PLAYER_HUMAN_H
#define PLAYER_HUMAN_H

#include <QObject>

class PlayerHuman : public QObject
{
    Q_OBJECT
public:
    explicit PlayerHuman(QObject* parent = 0);
};

#endif // PLAYER_HUMAN_H
