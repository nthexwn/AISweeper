#include "../inc/sweeper_control_window.h"
#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_widget.h"
#include "ui_sweeper_control_window.h"

SweeperControlWindow::SweeperControlWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::SweeperControlWindow)
{
    ui->setupUi(this);
    batchManager = new SweeperBatchManager(this);
    batchSettings = new SweeperBatchSettings(this);
    lastUsedBatchSettings = new SweeperBatchSettings(this);
    defaultBatchSettings = new SweeperBatchSettings(this);
    batchStatus = new SweeperBatchStatus(this);
    connect(batchManager, SIGNAL(triggerUpdateOverview(SweeperBatchStatus)),
            this, SLOT(doUpdateOverview(SweeperBatchStatus)));
}

SweeperControlWindow::~SweeperControlWindow()
{
    delete ui;
    ui = nullptr;
    SweeperWidget::unhookResources();
}

void SweeperControlWindow::doUpdateOverview(SweeperBatchStatus* batchStatus)
{
    // Update state
    switch(batchStatus->batchState)
    {
        case SweeperBatchStatus::NOT_STARTED:
            ui->batchStateLineEdit->setText("Not Started");
            SweeperCommonFunctions::setWidgetColors((QWidget*)ui->batchStateLineEdit, Qt::white, Qt::black);
            break;
        case SweeperBatchStatus::IN_PROGRESS:
            ui->batchStateLineEdit->setText("In Progress");
            SweeperCommonFunctions::setWidgetColors((QWidget*)ui->batchStateLineEdit, Qt::blue, Qt::black);
            break;
        case SweeperBatchStatus::COMPLETED:
            ui->batchStateLineEdit->setText("Completed");
            SweeperCommonFunctions::setWidgetColors((QWidget*)ui->batchStateLineEdit, Qt::green, Qt::black);
            break;
        case SweeperBatchStatus::TERMINATED:
            ui->batchStateLineEdit->setText("Terminated");
            SweeperCommonFunctions::setWidgetColors((QWidget*)ui->batchStateLineEdit, Qt::red, Qt::black);
            break;
    }

    // Update games played
    QString gamesPlayedString = (QString)batchStatus->gamesPlayed + "/" + (QString)batchStatus->totalGames;
    ui->batchStateLineEdit->setText(gamesPlayedString);

    // Update games won
    QString gamesWonString = (QString)batchStatus->gamesWon + "/" + (QString)batchStatus->gamesPlayed;
    if(batchStatus->gamesPlayed != 0)
    {
        int gamesWonPercentage = 100 * batchStatus->gamesWon / batchStatus->gamesPlayed;
        gamesWonString += " (" + (QString)gamesWonPercentage + "%)";
    }

    // Update progress
    ui->batchProgressBar->setValue(batchStatus->gamesPlayed / batchStatus->totalGames);
}

void SweeperControlWindow::disableSettings()
{
    ui->advancedRadioButton->setEnabled(false);
    ui->beginnerRadioButton->setEnabled(false);
    ui->customRadioButton->setEnabled(false);
    ui->gameCountLabel->setEnabled(false);
    ui->gameCountSpinBox->setEnabled(false);
    ui->heightLabel->setEnabled(false);
    ui->heightSpinBox->setEnabled(false);
    ui->intermediateRadioButton->setEnabled(false);
    ui->minesLabel->setEnabled(false);
    ui->minesSpinBox->setEnabled(false);
    ui->pausePerActionLabel->setEnabled(false);
    ui->pausePerActionSpinBox->setEnabled(false);
    ui->playerTypeComboBox->setEnabled(false);
    ui->playerTypeLabel->setEnabled(false);
    ui->restoreDefaultSettingsButton->setEnabled(false);
    ui->restoreLastUsedSettingsButton->setEnabled(false);
    ui->showGuiCheckBox->setEnabled(false);
    ui->unlockGuiCheckBox->setEnabled(false);
    ui->widthLabel->setEnabled(false);
    ui->widthSpinBox->setEnabled(false);
    ui->batchButton->setText("Terminate");
    SweeperCommonFunctions::setWidgetColors(ui->batchButton, Qt::green, Qt::white);
}

void SweeperControlWindow::enableSettings()
{
    ui->advancedRadioButton->setEnabled(true);
    ui->beginnerRadioButton->setEnabled(true);
    ui->customRadioButton->setEnabled(true);
    ui->gameCountLabel->setEnabled(true);
    ui->gameCountSpinBox->setEnabled(true);
    ui->heightLabel->setEnabled(true);
    ui->heightSpinBox->setEnabled(true);
    ui->intermediateRadioButton->setEnabled(true);
    ui->minesLabel->setEnabled(true);
    ui->minesSpinBox->setEnabled(true);
    ui->pausePerActionLabel->setEnabled(true);
    ui->pausePerActionSpinBox->setEnabled(true);
    ui->playerTypeComboBox->setEnabled(true);
    ui->playerTypeLabel->setEnabled(true);
    ui->restoreDefaultSettingsButton->setEnabled(true);
    ui->restoreLastUsedSettingsButton->setEnabled(true);
    ui->showGuiCheckBox->setEnabled(true);
    ui->unlockGuiCheckBox->setEnabled(true);
    ui->widthLabel->setEnabled(true);
    ui->widthSpinBox->setEnabled(true);
    ui->batchButton->setText("Launch");
    SweeperCommonFunctions::setWidgetColors(ui->batchButton, Qt::red, Qt::white);
}

void SweeperControlWindow::on_advancedRadioButton_clicked()
{
    ui->beginnerRadioButton->setChecked(false);
    ui->intermediateRadioButton->setChecked(false);
    ui->advancedRadioButton->setChecked(true);
    ui->customRadioButton->setChecked(false);
    ui->widthSpinBox->setEnabled(false);
    ui->widthSpinBox->setValue(30);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(16);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(99);
}

void SweeperControlWindow::on_batchButton_clicked()
{
    switch(batchStatus->batchState)
    {
        case SweeperBatchStatus::IN_PROGRESS:
            batchManager->terminateBatch();
            enableSettings();
            break;
        default:
            disableSettings();
            batchSettings->copyTo(lastUsedBatchSettings);
            batchManager->launchBatch(batchSettings);
            break;
    }
}

void SweeperControlWindow::on_beginnerRadioButton_clicked()
{
    ui->beginnerRadioButton->setChecked(true);
    ui->intermediateRadioButton->setChecked(false);
    ui->advancedRadioButton->setChecked(false);
    ui->customRadioButton->setChecked(false);
    ui->widthSpinBox->setEnabled(false);
    ui->widthSpinBox->setValue(9);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(9);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(10);
}

void SweeperControlWindow::on_customRadioButton_clicked()
{
    ui->beginnerRadioButton->setChecked(false);
    ui->intermediateRadioButton->setChecked(false);
    ui->advancedRadioButton->setChecked(false);
    ui->customRadioButton->setChecked(true);
    ui->widthSpinBox->setEnabled(true);
    ui->heightSpinBox->setEnabled(true);
    ui->minesSpinBox->setEnabled(true);
}

void SweeperControlWindow::on_gameCountSpinBox_valueChanged(int gameCount)
{
    batchSettings->gameCount = gameCount;
}

void SweeperControlWindow::on_heightSpinBox_valueChanged(int height)
{
    batchSettings->height = height;
}

void SweeperControlWindow::on_intermediateRadioButton_clicked()
{
    ui->beginnerRadioButton->setChecked(false);
    ui->intermediateRadioButton->setChecked(true);
    ui->advancedRadioButton->setChecked(false);
    ui->customRadioButton->setChecked(false);
    ui->widthSpinBox->setEnabled(false);
    ui->widthSpinBox->setValue(16);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(16);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(40);
}

void SweeperControlWindow::on_maxThreadCountSpinBox_valueChanged(int maxThreadCount)
{
    batchSettings->maxThreadCount = maxThreadCount;
}

void SweeperControlWindow::on_minesSpinBox_valueChanged(int mines)
{
    batchSettings->mines = mines;
}

void SweeperControlWindow::on_pausePerActionSpinBox_valueChanged(double secondsPausePerAction)
{
    batchSettings->msPausePerAction = secondsPausePerAction / 1000;
}

void SweeperControlWindow::on_playerTypeComboBox_activated(int playerTypeIndex)
{
    batchSettings->playerType = SweeperBatchSettings::PLAYER_TYPE(playerTypeIndex);
    if(batchSettings->playerType == SweeperBatchSettings::HUMAN)
    {
        ui->unlockGuiCheckBox->setChecked(true);
        batchSettings->unlockGui = true;
    }
    else
    {
        ui->unlockGuiCheckBox->setChecked(false);
        batchSettings->unlockGui = false;
    }
}

void SweeperControlWindow::on_playerTypeComboBox_currentIndexChanged(int playerTypeIndex)
{
    batchSettings->playerType = SweeperBatchSettings::PLAYER_TYPE(playerTypeIndex);
    if(batchSettings->playerType == SweeperBatchSettings::HUMAN)
    {
        ui->unlockGuiCheckBox->setChecked(true);
        batchSettings->unlockGui = true;
    }
    else
    {
        ui->unlockGuiCheckBox->setChecked(false);
        batchSettings->unlockGui = false;
    }
}

void SweeperControlWindow::on_restoreDefaultSettingsButton_clicked()
{
    defaultBatchSettings->copyTo(batchSettings);
    restoreSettingsHelper();
}

void SweeperControlWindow::on_restoreLastUsedSettingsButton_clicked()
{
    lastUsedBatchSettings->copyTo(batchSettings);
    restoreSettingsHelper();
}

void SweeperControlWindow::on_showGuiCheckBox_toggled(bool showGui)
{
    batchSettings->showGui = showGui;
}

void SweeperControlWindow::on_unlockGuiCheckBox_toggled(bool unlockGui)
{
    batchSettings->unlockGui = unlockGui;
}

void SweeperControlWindow::on_widthSpinBox_valueChanged(int width)
{
    batchSettings->width = width;
}

void SweeperControlWindow::restoreSettingsHelper()
{
    ui->advancedRadioButton->setChecked(false);
    ui->beginnerRadioButton->setChecked(false);
    ui->customRadioButton->setChecked(true);
    ui->gameCountSpinBox->setValue(batchSettings->gameCount);
    ui->heightSpinBox->setValue(batchSettings->height);
    ui->heightSpinBox->setEnabled(true);
    ui->intermediateRadioButton->setChecked(false);
    ui->maxThreadCountSpinBox->setValue(batchSettings->maxThreadCount);
    ui->minesSpinBox->setValue(batchSettings->mines);
    ui->minesSpinBox->setEnabled(true);
    ui->pausePerActionSpinBox->setValue(batchSettings->msPausePerAction);
    ui->playerTypeComboBox->setCurrentIndex(batchSettings->playerType);
    ui->showGuiCheckBox->setChecked(batchSettings->showGui);
    ui->unlockGuiCheckBox->setChecked(batchSettings->unlockGui);
    ui->widthSpinBox->setValue(batchSettings->width);
    ui->widthSpinBox->setEnabled(true);
}
