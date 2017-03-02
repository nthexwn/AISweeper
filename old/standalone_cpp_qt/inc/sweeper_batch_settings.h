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
    const static int BEGINNER_WIDTH = 9;
    const static int BEGINNER_HEIGHT = 9;
    const static int BEGINNER_MINES = 10;
    const static int INTERMEDIATE_WIDTH = 16;
    const static int INTERMEDIATE_HEIGHT = 16;
    const static int INTERMEDIATE_MINES = 40;
    const static int ADVANCED_WIDTH = 30;
    const static int ADVANCED_HEIGHT = 16;
    const static int ADVANCED_MINES = 99;
    int gameCount;
    int height;
    int maxThreadCount;
    int mines;
    PLAYER_TYPE playerType;
    unsigned long msPausePerAction;
    bool showGui;
    bool unlockGui;
    int width;
    explicit SweeperBatchSettings(QObject* parent = 0);
    SweeperBatchSettings(SweeperBatchSettings* batchSettings, QObject* parent = 0);
    void copyTo(SweeperBatchSettings* batchSettings);
};

#endif // SWEEPER_BATCH_SETTINGS_H
