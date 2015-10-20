#ifndef SWEEPER_BATCH_MANAGER_H
#define SWEEPER_BATCH_MANAGER_H

#include <QObject>
#include "sweeper_batch_status.h"
#include "sweeper_batch_settings.h"
#include "sweeper_game.h"

class SweeperBatchManager : public QObject
{
    Q_OBJECT
public:
    explicit SweeperBatchManager(QObject* parent = 0);
    ~SweeperBatchManager();
    void launchBatch(SweeperBatchSettings* batchSettings);
    void requestBatchTermination();

signals:
    void triggerUpdateOverview(SweeperBatchStatus* batchStatus);

private:
    SweeperBatchStatus* batchStatus;
    bool batchTerminationRequested;
    SweeperGame* sweeperGame;
};

#endif // SWEEPER_BATCH_MANAGER_H
