#include "../inc/sweeper_batch_settings.h"

SweeperBatchSettings::SweeperBatchSettings(QObject* parent) : QObject(parent)
{
    gameCount = 1;
    height = 9;
    maxThreadCount = 1;
    mines = 10;
    playerType = SweeperBatchSettings::HUMAN;
    secondsPausePerAction = 0.000;
    showGui = true;
    unlockGui = true;
    width = 9;
}

SweeperBatchSettings::SweeperBatchSettings(SweeperBatchSettings* batchSettings, QObject* parent) : QObject(parent)
{
    gameCount = batchSettings->gameCount;
    height = batchSettings->height;
    maxThreadCount = batchSettings->maxThreadCount;
    mines = batchSettings->mines;
    playerType = batchSettings->playerType;
    secondsPausePerAction = batchSettings->secondsPausePerAction;
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
    batchSettings->secondsPausePerAction = secondsPausePerAction;
    batchSettings->showGui = showGui;
    batchSettings->unlockGui = unlockGui;
    batchSettings->width = width;
}
