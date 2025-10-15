#ifndef MAINMENUCONTROLLER_H
#define MAINMENUCONTROLLER_H

#include <QObject>
#include "menuviewmodel.h"

class MainMenuController : public QObject
{
    Q_OBJECT
public:
    explicit MainMenuController(QObject *parent = nullptr);
    void initialize();

public slots:
    // API for the root controller to use
    void show();
    void hide();

    // Input handling
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onSelectButtonPressed();
    void onBackButtonPressed(); // Add back button handling

signals:
    // Events for the root controller to listen to
    void personalizeReticleRequested();
    void personalizeColorsRequested();
    void adjustBrightnessRequested();
    void zeroingRequested();
    void clearZeroRequested();
    void windageRequested();
    void clearWindageRequested();
    void zoneDefinitionsRequested();
    void systemStatusRequested();
    void helpAboutRequested();
    void radarTargetListRequested(); // If you have radar functionality

    void menuFinished();

private slots:
    void handleMenuOptionSelected(const QString& option);

private:
    MenuViewModel* m_viewModel;

    // Helper to build menu options
    QStringList buildMainMenuOptions() const;
};

#endif // MAINMENUCONTROLLER_H
