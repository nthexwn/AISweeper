#ifndef PLAYER_ABSTRACT_H
#define PLAYER_ABSTRACT_H

#include <QObject>
#include "../sweeper_common_functions.h"
#include "../sweeper_model.h"

class PlayerAbstract : public QObject
{
    Q_OBJECT
public:
    explicit PlayerAbstract(SweeperModel* sweeperModel, QObject* parent = 0);
    virtual void takeNextAction() = 0;

signals:
    triggerFlagAction(QPoint);
    triggerQuitAction();
    triggerRevealAction(QPoint);
    triggerRevealAdjacentAction(QPoint);

public slots:
    void doTakeNextAction();

protected:
    SweeperModel* sweeperModel;
};

#endif // PLAYER_ABSTRACT_H
