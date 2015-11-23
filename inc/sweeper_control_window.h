#ifndef SWEEPER_CONTROL_WINDOW_H
#define SWEEPER_CONTROL_WINDOW_H

#include <QMainWindow>
#include "sweeper_batch_manager.h"

// Boilerplate namespace entry for Qt.  Not sure why we need this.
namespace Ui { class SweeperControlWindow; }

class SweeperControlWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SweeperControlWindow(QWidget* parent = 0);
    ~SweeperControlWindow();

signals:
    // These will be used to control the batchManager.
    void triggerLaunchBatch(SweeperBatchSettings* batchSettings);
    void triggerTerminateBatch();

public slots:
    // These slots will be triggered by the batchManager before and after every batch execution.  They are used to
    // update the GUI elements (IE: the launch button) and enable/disable the settings elements as appropriate.
    void doBatchDone();
    void doBatchLaunched();

    // We'll connect this slot to the batchManager when the control window's constructor is called during application
    // start.  The batchManager should send the latest status at this time so that the control window can use it to
    // update the "Batch Overview" section of the control window's GUI.
    void doUpdateOverview(SweeperBatchStatus* latestBatchStatus);

private:
    // Qt generated pointer that allows us to access UI elements declared in the form file for this class.  The form
    // file is/was edited graphically in the Qt Creator application and saved as XML data in a ".ui" file.  That XML
    // data is only merged with the ".cpp" files (like the one you're reading now) at compile time.  This pointer is
    // the only way for us to refer to this class' graphically edited form elements from within the C++ code.
    Ui::SweeperControlWindow *ui;

    // There should only be one batch manager instantiated at a time.  We will instantiate it and connect it to the
    // control window when the control window's constructor is called during application start.
    SweeperBatchManager* batchManager;

    // This is where all of the settings for the current batch will be stored.  Every time the user updates the
    // settings on the GUI they will be reflected here.  These settings will then be passed to the batchManager when
    // starting a new batch of games.
    SweeperBatchSettings* batchSettings;

    // This will point to a copy of the last batch settings which were used.  These may be restored by the user by
    // cliking on the "restore last settings used" button on the GUI.
    SweeperBatchSettings* lastUsedBatchSettings;

    // This will point to a copy of the default batch settings to be used.  These may be restored by the user by
    // clicking on the "restore default settings" button on the GUI.
    SweeperBatchSettings* defaultBatchSettings;

    // This status will be updated whenever the batchManager signals that it needs to be updated.  It contains all of
    // the data needed to populate the "Batch Overview" section of the control window's GUI.
    SweeperBatchStatus* batchStatus;

    // We don't want the user adjusting settings while the program is running, so we'll disable all of the GUI
    // controls until the batch has completed or been terminated.
    void disableSettings();

    // Once the current batch has been completed or terminated we will re-enable the GUI controls so the user can
    // adjust settings prior to the next batch if desired.
    void enableSettings();

    // Both restoring default settings and restoring the last used settings require that the GUI be manually reset.
    // This is a helper function that prevents duplicated code in the two event handlers that perform those actions.
    void restoreSettingsHelper();

private slots:
    // These slots were automatically generated by Qt.  They function as event handlers for the GUI.  They are
    // primarily used to assign batch settings as they are selected by the user.
    void on_advancedRadioButton_clicked();
    void on_batchButton_clicked();
    void on_beginnerRadioButton_clicked();
    void on_customRadioButton_clicked();
    void on_gameCountSpinBox_valueChanged(int gameCount);
    void on_heightSpinBox_valueChanged(int height);
    void on_intermediateRadioButton_clicked();
    void on_maxThreadCountSpinBox_valueChanged(int maxThreadCount);
    void on_minesSpinBox_valueChanged(int mines);
    void on_pausePerActionSpinBox_valueChanged(double secondsPausePerAction);
    void on_playerTypeComboBox_activated(int playerTypeIndex);
    void on_playerTypeComboBox_currentIndexChanged(int playerTypeIndex);
    void on_restoreDefaultSettingsButton_clicked();
    void on_restoreLastUsedSettingsButton_clicked();
    void on_showGuiCheckBox_toggled(bool showGui);
    void on_unlockGuiCheckBox_toggled(bool unlockGui);
    void on_widthSpinBox_valueChanged(int width);
};

#endif // SWEEPER_CONTROL_WINDOW_H
