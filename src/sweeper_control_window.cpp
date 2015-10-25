#include "../inc/sweeper_control_window.h"
#include "../inc/sweeper_common_functions.h"
#include "../inc/sweeper_widget.h"
#include "ui_sweeper_control_window.h"

SweeperControlWindow::SweeperControlWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::SweeperControlWindow)
{
    ui->setupUi(this);
    ui->batchButton->setStyleSheet("background-color: green");
    batchManager = new SweeperBatchManager(this);
    batchSettings = new SweeperBatchSettings(this);
    batchStatus = new SweeperBatchStatus(this);
    lastUsedBatchSettings = new SweeperBatchSettings(this);
    defaultBatchSettings = new SweeperBatchSettings(this);
    connect(batchManager, SIGNAL(triggerUpdateOverview(SweeperBatchStatus*)),
            this, SLOT(doUpdateOverview(SweeperBatchStatus*)));
}

SweeperControlWindow::~SweeperControlWindow()
{
    delete ui;
    SweeperWidget::unhookResources();
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
    ui->maxThreadCountLabel->setEnabled(false);
    ui->maxThreadCountSpinBox->setEnabled(false);
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
    ui->batchButton->setStyleSheet("background-color: red");
    ui->batchButton->setText("Terminate");
    update();
}

void SweeperControlWindow::enableSettings()
{
    ui->advancedRadioButton->setEnabled(true);
    ui->beginnerRadioButton->setEnabled(true);
    ui->customRadioButton->setEnabled(true);
    if(ui->customRadioButton->isChecked())
    {
        ui->widthSpinBox->setEnabled(true);
        ui->heightSpinBox->setEnabled(true);
        ui->minesSpinBox->setEnabled(true);
    }
    ui->gameCountLabel->setEnabled(true);
    ui->gameCountSpinBox->setEnabled(true);
    ui->heightLabel->setEnabled(true);
    ui->intermediateRadioButton->setEnabled(true);
    ui->maxThreadCountLabel->setEnabled(true);
    ui->maxThreadCountSpinBox->setEnabled(true);
    ui->minesLabel->setEnabled(true);
    ui->pausePerActionLabel->setEnabled(true);
    ui->pausePerActionSpinBox->setEnabled(true);
    ui->playerTypeComboBox->setEnabled(true);
    ui->playerTypeLabel->setEnabled(true);
    ui->restoreDefaultSettingsButton->setEnabled(true);
    ui->restoreLastUsedSettingsButton->setEnabled(true);
    ui->showGuiCheckBox->setEnabled(true);
    ui->widthLabel->setEnabled(true);
    ui->batchButton->setStyleSheet("background-color: green");
    ui->batchButton->setText("Launch");
    update();
}

void SweeperControlWindow::restoreSettingsHelper()
{
    if(batchSettings->width == SweeperBatchSettings::BEGINNER_WIDTH &&
       batchSettings->height == SweeperBatchSettings::BEGINNER_HEIGHT &&
       batchSettings->mines == SweeperBatchSettings::BEGINNER_MINES)
    {
        on_beginnerRadioButton_clicked();
    }
    else if(batchSettings->width == SweeperBatchSettings::INTERMEDIATE_WIDTH &&
            batchSettings->height == SweeperBatchSettings::INTERMEDIATE_HEIGHT &&
            batchSettings->mines == SweeperBatchSettings::INTERMEDIATE_MINES)
    {
        on_intermediateRadioButton_clicked();
    }
    else if(batchSettings->width == SweeperBatchSettings::ADVANCED_WIDTH &&
            batchSettings->height == SweeperBatchSettings::ADVANCED_HEIGHT &&
            batchSettings->mines == SweeperBatchSettings::ADVANCED_MINES)
    {
        on_advancedRadioButton_clicked();
    }
    else
    {
        on_customRadioButton_clicked();
        ui->widthSpinBox->setValue(batchSettings->width);
        ui->heightSpinBox->setValue(batchSettings->height);
        ui->minesSpinBox->setValue(batchSettings->mines);
    }
    ui->gameCountSpinBox->setValue(batchSettings->gameCount);
    ui->maxThreadCountSpinBox->setValue(batchSettings->maxThreadCount);
    ui->pausePerActionSpinBox->setValue((double)batchSettings->msPausePerAction / 1000);
    ui->playerTypeComboBox->setCurrentIndex(batchSettings->playerType);
    ui->showGuiCheckBox->setChecked(batchSettings->showGui);
    ui->unlockGuiCheckBox->setChecked(batchSettings->unlockGui);
    update();
}

void SweeperControlWindow::doUpdateOverview(SweeperBatchStatus* latestBatchStatus)
{
    latestBatchStatus->copyTo(batchStatus);

    // Update state
    switch(batchStatus->batchState)
    {
        case SweeperBatchStatus::NOT_STARTED:
            ui->batchStateLineEdit->setText("Not Started");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::white, Qt::black);
            break;
        case SweeperBatchStatus::IN_PROGRESS:
            ui->batchStateLineEdit->setText("In Progress");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::blue, Qt::white);
            break;
        case SweeperBatchStatus::COMPLETED:
            ui->batchStateLineEdit->setText("Completed");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::green, Qt::black);
            break;
        case SweeperBatchStatus::TERMINATED:
            ui->batchStateLineEdit->setText("Terminated");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::red, Qt::black);
            break;
    }

    // Update games played
    QString gamesPlayedString = QString::number(batchStatus->gamesPlayed) %
                                "/" %
                                QString::number(batchStatus->totalGames);
    ui->batchGamesPlayedLineEdit->setText(gamesPlayedString);

    // Update games won
    QString gamesWonString;
    if(batchStatus->gamesPlayed != 0)
    {
        int gamesWonPercentage = 100 * batchStatus->gamesWon / batchStatus->gamesPlayed;
        gamesWonString = QString::number(batchStatus->gamesWon) %
                         "/" %
                         QString::number(batchStatus->gamesPlayed) %
                         " (" %
                         QString::number(gamesWonPercentage) %
                         "%)";
    }
    else
    {
        gamesWonString = "0/0";
    }
    ui->batchGamesWonLineEdit->setText(gamesWonString);

    // Update progress
    ui->batchProgressBar->setValue(batchStatus->gamesPlayed / batchStatus->totalGames);

    // Force the GUI to refresh
    update();
}

void SweeperControlWindow::on_advancedRadioButton_clicked()
{
    ui->beginnerRadioButton->setChecked(false);
    ui->intermediateRadioButton->setChecked(false);
    ui->advancedRadioButton->setChecked(true);
    ui->customRadioButton->setChecked(false);
    ui->widthSpinBox->setEnabled(false);
    ui->widthSpinBox->setValue(SweeperBatchSettings::ADVANCED_WIDTH);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(SweeperBatchSettings::ADVANCED_HEIGHT);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(SweeperBatchSettings::ADVANCED_MINES);
    update();
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
    ui->widthSpinBox->setValue(SweeperBatchSettings::BEGINNER_WIDTH);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(SweeperBatchSettings::BEGINNER_HEIGHT);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(SweeperBatchSettings::BEGINNER_MINES);
    update();
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
    update();
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
    ui->widthSpinBox->setValue(SweeperBatchSettings::INTERMEDIATE_WIDTH);
    ui->heightSpinBox->setEnabled(false);
    ui->heightSpinBox->setValue(SweeperBatchSettings::INTERMEDIATE_HEIGHT);
    ui->minesSpinBox->setEnabled(false);
    ui->minesSpinBox->setValue(SweeperBatchSettings::INTERMEDIATE_MINES);
    update();
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
    batchSettings->msPausePerAction = secondsPausePerAction * 1000;
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
