#include "controllers/colormenucontroller.h"
#include "services/servicemanager.h"
#include "models/osdviewmodel.h"
#include <QDebug>

ColorMenuController::ColorMenuController(QObject *parent)
    : QObject(parent)
    , m_viewModel(nullptr)
    , m_osdViewModel(nullptr)
    , m_originalColorStyle(ColorStyle::Green)
{
}

void ColorMenuController::initialize()
{
    // Get the COLOR menu's ViewModel specifically by name
    m_viewModel = ServiceManager::instance()->get<MenuViewModel>(QString("ColorMenuViewModel"));
    m_osdViewModel = ServiceManager::instance()->get<OsdViewModel>();

    Q_ASSERT(m_viewModel);
    Q_ASSERT(m_osdViewModel);

    connect(m_viewModel, &MenuViewModel::optionSelected,
            this, &ColorMenuController::handleMenuOptionSelected);
}

QStringList ColorMenuController::buildColorOptions() const
{
    QStringList options;

    for (int i = 0; i < static_cast<int>(ColorStyle::COUNT); ++i) {
        options << colorStyleToString(static_cast<ColorStyle>(i));
    }

    options << "Return ...";
    return options;
}

QString ColorMenuController::colorStyleToString(ColorStyle style) const
{
    switch(style) {
    case ColorStyle::Green: return "Green";
    case ColorStyle::Red: return "Red";
    case ColorStyle::White: return "White";
    //case ColorStyle::Yellow: return "Yellow";
    //case ColorStyle::Cyan: return "Cyan";
    default: return "Green";
    }
}

ColorStyle ColorMenuController::stringToColorStyle(const QString& str) const
{
    if (str == "Green") return ColorStyle::Green;
    if (str == "Red") return ColorStyle::Red;
    if (str == "White") return ColorStyle::White;
    //if (str == "Yellow") return ColorStyle::Yellow;
    //if (str == "Cyan") return ColorStyle::Cyan;
    return ColorStyle::Green;
}

QColor ColorMenuController::colorStyleToQColor(ColorStyle style) const
{
    switch(style) {
    case ColorStyle::Green: return QColor("#00FF99");
    case ColorStyle::Red: return QColor("#FF0000");
    case ColorStyle::White: return QColor("#FFFFFF");
    //case ColorStyle::Yellow: return QColor("#FFFF00");
    //case ColorStyle::Cyan: return QColor("#00FFFF");
    default: return QColor("#00FF99");
    }
}

void ColorMenuController::show()
{
    // Save current color
    // m_originalColorStyle = m_osdViewModel->getCurrentColorStyle();

    QStringList options = buildColorOptions();
    m_viewModel->showMenu("Personalize Colors", "Select OSD Color", options);

    // Set current selection
    // int currentIndex = static_cast<int>(m_originalColorStyle);
    // if (currentIndex >= 0 && currentIndex < options.size()) {
    //     m_viewModel->setCurrentIndex(currentIndex);
    // }
}

void ColorMenuController::hide()
{
    m_viewModel->hideMenu();
}

void ColorMenuController::onUpButtonPressed()
{
    m_viewModel->moveSelectionUp();

    int currentIndex = m_viewModel->currentIndex();
    handleCurrentItemChanged(currentIndex);
}

void ColorMenuController::onDownButtonPressed()
{
    m_viewModel->moveSelectionDown();

    int currentIndex = m_viewModel->currentIndex();
    handleCurrentItemChanged(currentIndex);
}

void ColorMenuController::onSelectButtonPressed()
{
    m_viewModel->selectCurrentItem();
}

void ColorMenuController::onBackButtonPressed()
{
    // Restore original color on cancel
    // QColor originalColor = colorStyleToQColor(m_originalColorStyle);
    // m_osdViewModel->setOsdColor(originalColor);

    hide();
    emit returnToMainMenu();
    emit menuFinished();
}

void ColorMenuController::handleCurrentItemChanged(int index)
{
    QStringList options = buildColorOptions();
    if (index >= 0 && index < options.size() - 1) {
        QString optionText = options.at(index);
        ColorStyle previewStyle = stringToColorStyle(optionText);
        QColor previewColor = colorStyleToQColor(previewStyle);

        // Update OSD with preview
        // m_osdViewModel->setOsdColor(previewColor);

        qDebug() << "ColorMenuController: Previewing" << optionText;
    }
}

void ColorMenuController::handleMenuOptionSelected(const QString& option)
{
    qDebug() << "ColorMenuController: Selected" << option;

    hide();

    if (option == "Return ...") {
        // Restore original
        // QColor originalColor = colorStyleToQColor(m_originalColorStyle);
        // m_osdViewModel->setOsdColor(originalColor);
        emit returnToMainMenu();
    } else {
        // Apply selected color permanently
        ColorStyle selectedStyle = stringToColorStyle(option);
        QColor selectedColor = colorStyleToQColor(selectedStyle);

        // m_osdViewModel->setOsdColor(selectedColor);
        // m_osdViewModel->saveColorStyle(); // Persist

        qDebug() << "ColorMenuController: Applied" << option << selectedColor;
        emit returnToMainMenu();
    }

    emit menuFinished();
}
