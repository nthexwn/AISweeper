#include "../inc/sweeper_batch_settings.h"

SweeperBatchSettings::SweeperBatchSettings(QObject* parent) : QObject(parent)
{
    gameCount = 1;
    height = SweeperBatchSettings::BEGINNER_HEIGHT;
    maxThreadCount = 1;
    mines = SweeperBatchSettings::BEGINNER_MINES;
    playerType = SweeperBatchSettings::HUMAN;
    msPausePerAction = 0;
    showGui = true;
    unlockGui = true;
    width = SweeperBatchSettings::BEGINNER_WIDTH;
}

SweeperBatchSettings::SweeperBatchSettings(SweeperBatchSettings* batchSettings, QObject* parent) : QObject(parent)
{
    gameCount = batchSettings->gameCount;
    height = batchSettings->height;
    maxThreadCount = batchSettings->maxThreadCount;
    mines = batchSettings->mines;
    playerType = batchSettings->playerType;
    msPausePerAction = batchSettings->msPausePerAction;
    showGui = batchSettings->showGui;
    unlockGui = batchSettings->unlockGui;
    width = batchSettings->width;
}

void SweeperBatchSettings::copyTo(SweeperBatchSettings* batchSettings)
{
    batchSettings->gameCount = gameCount;
    batchSettings->height = height;
    batchSettings->maxThreadCount = maxThreadCount;
    batchSettings->mines = mines;
    batchSettings->playerType = playerType;
    batchSettings->msPausePerAction = msPausePerAction;
    batchSettings->showGui = showGui;
    batchSettings->unlockGui = unlockGui;
    batchSettings->width = width;
}
