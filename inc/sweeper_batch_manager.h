#ifndef SWEEPER_BATCH_MANAGER_H
#define SWEEPER_BATCH_MANAGER_H

#include <QMutex>
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
    void terminateBatch();

signals:
    void triggerBeginGame();
    void triggerUpdateOverview(SweeperBatchStatus* batchStatus);

public slots:
    void doEndGame(SweeperGame* sweeperGame);

private:
    QThread** threads;
    int threadsInBatch;
    SweeperBatchSettings* batchSettings;
    SweeperBatchStatus* batchStatus;
};

#endif // SWEEPER_BATCH_MANAGER_H
