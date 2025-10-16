#ifndef MAINMENUCONTROLLER_H
#define MAINMENUCONTROLLER_H

#include <QObject>
#include "models/menuviewmodel.h"

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

    // Input handling - Only 3 buttons now
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onSelectButtonPressed();  // Called by MENU/VAL when in main menu

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
    void radarTargetListRequested();

    void menuFinished();

private slots:
    void handleMenuOptionSelected(const QString& option);

private:
    MenuViewModel* m_viewModel;

    // Helper to build menu options
    QStringList buildMainMenuOptions() const;
};

#endif // MAINMENUCONTROLLER_H
