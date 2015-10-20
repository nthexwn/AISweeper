#ifndef SWEEPER_BATCH_SETTINGS_H
#define SWEEPER_BATCH_SETTINGS_H

#include <QObject>

class SweeperBatchSettings : public QObject
{
    Q_OBJECT
public:
    enum PLAYER_TYPE
    {
        HUMAN,
        RANDOM_ACTION_AI,
        PROBABILITY_BASED_AI,
        MACHINE_LEARNING_AI
    };
    int gameCount;
    int height;
    int maxThreadCount;
    int mines;
    PLAYER_TYPE playerType;
    double secondsPausePerAction;
    bool showGui;
    bool unlockGui;
    int width;
    explicit SweeperBatchSettings(QObject* parent = 0);
    SweeperBatchSettings(SweeperBatchSettings* batchSettings, QObject* parent = 0);
    void copyTo(SweeperBatchSettings* batchSettings);
};

#endif // SWEEPER_BATCH_SETTINGS_H
