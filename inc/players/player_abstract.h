#ifndef PLAYER_ABSTRACT_H
#define PLAYER_ABSTRACT_H

#include <QObject>

class PlayerAbstract : public QObject
{
    Q_OBJECT
public:
    explicit PlayerAbstract(QObject* parent = 0);
};

#endif // PLAYER_ABSTRACT_H
