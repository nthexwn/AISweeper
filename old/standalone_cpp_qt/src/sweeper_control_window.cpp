#include "../inc/sweeper_control_window.h"
#include "../inc/sweeper_common_functions.h"
#include "ui_sweeper_control_window.h"

// Constructor called from main function
SweeperControlWindow::SweeperControlWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::SweeperControlWindow)
{
    // This will be set to true when the user closes the window
    terminationRequested = false;

    // Setup all of the GUI elements that were declared in the control window's form file (not in code)
    ui->setupUi(this);
    ui->batchButton->setStyleSheet("background-color: green");

    // Create the settings objects (we'll re-use these between batches).  Note that these are parented to the current
    // window so that we don't need to delete them ourselves in the destructor (QT handles this for us).
    batchSettings = new SweeperBatchSettings(this);
    batchStatus = new SweeperBatchStatus(this);
    lastUsedBatchSettings = new SweeperBatchSettings(this);
    defaultBatchSettings = new SweeperBatchSettings(this);

    // Instantiate the map which we'll use to track game widgets
    sweeperWidgets = new QHash<int, SweeperWidget*>();

    // Create the batch manager and assign it to its own thread
    batchManager = new SweeperBatchManager();
    batchManagerThread = new QThread();
    batchManager->moveToThread(batchManagerThread);

    // Connect the batch manager to the control window
    connect(batchManager, SIGNAL(triggerBatchDone()),
            this, SLOT(doBatchDone()));
    connect(batchManager, SIGNAL(triggerBatchLaunched()),
            this, SLOT(doBatchLaunched()));
    connect(batchManager, SIGNAL(triggerDeleteSweeperWidget(SweeperWidget*, int)),
            this, SLOT(doDeleteSweeperWidget(SweeperWidget*, int)));
    connect(batchManager, SIGNAL(triggerGenerateSweeperWidget(SweeperModel*, int)),
            this, SLOT(doGenerateSweeperWidget(SweeperModel*, int)));
    connect(batchManager, SIGNAL(triggerShowSweeperWidget(SweeperWidget*)),
            this, SLOT(doShowSweeperWidget(SweeperWidget*)));
    connect(batchManager, SIGNAL(triggerUpdateOverview(SweeperBatchStatus*)),
            this, SLOT(doUpdateOverview(SweeperBatchStatus*)));

    // Connect the control window to the batch manager
    connect(this, SIGNAL(triggerLaunchBatch(SweeperBatchSettings*)),
            batchManager, SLOT(doLaunchBatch(SweeperBatchSettings*)));
    connect(this, SIGNAL(triggerSweeperWidgetDeleted(int)),
            batchManager, SLOT(doSweeperWidgetDeleted(int)));
    connect(this, SIGNAL(triggerSweeperWidgetGenerated(SweeperWidget*, int)),
            batchManager, SLOT(doSweeperWidgetGenerated(SweeperWidget*, int)));
    connect(this, SIGNAL(triggerTerminateBatch()),
            batchManager, SLOT(doTerminateBatch()));

    // Start the batch manager thread
    batchManagerThread->start();
}

// The batch manager should already be deleted when this destructor is called and its thread scheduled for deletion.
SweeperControlWindow::~SweeperControlWindow()
{
    // Delete all of the GUI elements that were declared in the control window's form file (not in code)
    delete ui;

    // Unload all of the game tile images from memory
    SweeperWidget::unhookResources();

    // Shut down the program.
    QCoreApplication::quit();
}

void SweeperControlWindow::closeEvent(QCloseEvent* event)
{
    // The control window has been closed, so we'll shut the application down as soon as we can.
    event->accept();

    // As soon as the current batch finishes we'll delete the batch manager as well as this control window.  This is
    // the flag that tells us to do so.
    terminationRequested = true;

    // For now we'll tell the current batch to terminate itself and wait...  (until the doBatchDone slot is called).
    emit triggerTerminateBatch();
}

// Triggered by batch manager after all of the game objects for the batch have been stopped
void SweeperControlWindow::doBatchDone()
{
    // If we were waiting on the batch to finish/terminate before shutting down the rest of the program...
    if(terminationRequested)
    {
        // Since the last batch just finished we know it's safe to delete the batch manager and stop its thread.
        batchManager->deleteLater();
        batchManagerThread->quit();
        batchManagerThread->deleteLater();

        // We're now ready to delete the control window as well.
        this->deleteLater();
    }
    else
    {
        // Allow settings for the next batch to be modified
        enableSettings();

        // Allow the next batch to be started
        ui->batchButton->setEnabled(true);
        ui->batchButton->setStyleSheet("background-color: green; color: black");
        ui->batchButton->setText("Launch");

        // Render the changes in the control window GUI immediately
        update();
    }
}

// Triggered by batch manager after all of the game objects for the batch have been started
void SweeperControlWindow::doBatchLaunched()
{
    // Allow the batch to be stopped
    ui->batchButton->setEnabled(true);
    ui->batchButton->setStyleSheet("background-color: red; color: black");
    ui->batchButton->setText("Terminate");

    // Render the changes in the control window GUI immediately
    update();
}

void SweeperControlWindow::doGenerateSweeperWidget(SweeperModel* sweeperModel, int index)
{
    SweeperWidget* widget = new SweeperWidget(sweeperModel);
    emit triggerSweeperWidgetGenerated(widget, index);
}

void SweeperControlWindow::doShowSweeperWidget(int index)
{
    SweeperWidget* widget = sweeperWidgets->find(index).value();
    widget->show();
}

void SweeperControlWindow::doDeleteSweeperWidget(int index)
{
    // Since Qt forces all GUI objects to be handled in the main thread we have to delete the widgets for each game
    // here.
    delete sweeperWidgets->find(index).value();
    emit triggerSweeperWidgetDeleted(index);
}

// Triggered by batch manager whenever batch status information has been modified (IE: games completed count changes)
void SweeperControlWindow::doUpdateOverview(SweeperBatchStatus* latestBatchStatus)
{
    latestBatchStatus->copyTo(batchStatus);
    delete latestBatchStatus;

    // Update state
    switch(batchStatus->batchState)
    {
        case SweeperBatchStatus::NOT_STARTED:
            ui->batchStateLineEdit->setText("Not Started");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::white, Qt::black);
            break;
        case SweeperBatchStatus::LOADING:
            ui->batchStateLineEdit->setText("Loading...");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::blue, Qt::black);
            break;
        case SweeperBatchStatus::IN_PROGRESS:
            ui->batchStateLineEdit->setText("In Progress");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::blue, Qt::white);
            break;
        case SweeperBatchStatus::COMPLETED:
            ui->batchStateLineEdit->setText("Completed");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::green, Qt::black);
            break;
        case SweeperBatchStatus::TERMINATING:
            ui->batchStateLineEdit->setText("Terminating...");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::red, Qt::black);
            break;
        case SweeperBatchStatus::TERMINATED:
            ui->batchStateLineEdit->setText("Terminated");
            SweeperCommonFunctions::setWidgetPaletteColors((QWidget*)ui->batchStateLineEdit, Qt::red, Qt::black);
            break;
    }

    // Update games played (% is a Qt macro that replicates StringBuilder functionality for linked concatenation.
    // It's essentially the same as the + operator, but with better performance in many cases.)
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
        // We don't want to divide by zero, so we just display this instead.
        gamesWonString = "0/0";
    }
    ui->batchGamesWonLineEdit->setText(gamesWonString);

    // Update progress
    ui->batchProgressBar->setValue(batchStatus->gamesPlayed / batchStatus->totalGames);

    // Force the GUI to refresh
    update();
}

// This gets called immediately after the launch batch button is pushed by the user.  We don't want the batch settings
// to be modified while a batch is running.
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
    update();
}

// This gets called after the batch manager signals the end of the previous batch.  This means that it's safe to update
// the settings for the next batch since no batch is currently running.
void SweeperControlWindow::enableSettings()
{
    ui->advancedRadioButton->setEnabled(true);
    ui->beginnerRadioButton->setEnabled(true);
    ui->customRadioButton->setEnabled(true);
    if(ui->customRadioButton->isChecked())
    {
        // Only re-enable the custom game settings if the difficulty was set to custom
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
    update();
}

// This is shared code for restoring either default settings or the last used settings.
void SweeperControlWindow::restoreSettingsHelper()
{
    // The game difficulties are only shown on the GUI.  If the actual numeric values (height, width, and mine count)
    // happen to match up with one of the difficulties then we'll pretend that the user just selected that difficulty.
    // This will cause all of the related GUI updates to be handled for us (IE: deselecting the other difficulties).
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

    // Restoring the other settings is much more straight-forward.
    ui->gameCountSpinBox->setValue(batchSettings->gameCount);
    ui->maxThreadCountSpinBox->setValue(batchSettings->maxThreadCount);
    ui->pausePerActionSpinBox->setValue((double)batchSettings->msPausePerAction / 1000);
    ui->playerTypeComboBox->setCurrentIndex(batchSettings->playerType);
    ui->showGuiCheckBox->setChecked(batchSettings->showGui);
    ui->unlockGuiCheckBox->setChecked(batchSettings->unlockGui);
    update();
}

// Deselect the other difficulties and set the appropriate height/width/mines values
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

// Starts the next batch or stops the current one as appropriate
void SweeperControlWindow::on_batchButton_clicked()
{
    switch(batchStatus->batchState)
    {
        // If a batch is currently in progress then we'll terminate it.
        case SweeperBatchStatus::IN_PROGRESS:
            // Immediately disable the button so that we don't send multiple terminate signals
            ui->batchButton->setEnabled(false);
            ui->batchButton->setStyleSheet("background-color: blue; color: white");
            ui->batchButton->setText("Stopping...");

            // In addition to the regular GUI updates it appears that we need to trigger a repaint on OSX in order to
            // reflect the changes we've made to the batch button.
            repaint();
            update();

            // Send a signal to the batch manager to terminate the batch as soon as it can
            emit triggerTerminateBatch();
            break;

        // If no batch is currently in progress then we'll launch a new one.
        default:
            // Immediately disable the button so that we don't send multiple launch signals
            ui->batchButton->setEnabled(false);
            ui->batchButton->setStyleSheet("background-color: blue; color: white");
            ui->batchButton->setText("Loading...");
            disableSettings();

            // In addition to the regular GUI updates it appears that we need to trigger a repaint on OSX in order to
            // reflect the changes we've made to the batch button.
            repaint();
            update();

            // Save the batch settings that we're using for this batch
            batchSettings->copyTo(lastUsedBatchSettings);

            // Send a signal to the batch manager to launch the batch as soon as it can
            emit triggerLaunchBatch(batchSettings);
            break;
    }
}

// Deselect the other difficulties and set the appropriate height/width/mines values
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

// Deselect the other difficulties and allow the height/width/mines values to be customized
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

// Deselect the other difficulties and set the appropriate height/width/mines values
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
    // Disable the GUI for all of the AI players and leave it on for humans
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
    // Disable the GUI for all of the AI players and leave it on for humans
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
