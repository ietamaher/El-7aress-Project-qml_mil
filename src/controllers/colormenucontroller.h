#ifndef COLORMENUCONTROLLER_H
#define COLORMENUCONTROLLER_H

#include <QObject>
#include <QColor>
#include "models/menuviewmodel.h"
#include "models/domain/systemstatedata.h"
/*enum class ColorStyle {
    Green = 0,
    Red,
    White,
    Yellow,
    Cyan,
    COUNT
};*/

class OsdViewModel;

class ColorMenuController : public QObject
{
    Q_OBJECT
public:
    explicit ColorMenuController(QObject *parent = nullptr);
    void initialize();

public slots:
    void show();
    void hide();
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onSelectButtonPressed();
    void onBackButtonPressed();

signals:
    void menuFinished();
    void returnToMainMenu();

private slots:
    void handleMenuOptionSelected(const QString& option);
    void handleCurrentItemChanged(int index);

private:
    MenuViewModel* m_viewModel;
    OsdViewModel* m_osdViewModel;

    QStringList buildColorOptions() const;
    QString colorStyleToString(ColorStyle style) const;
    ColorStyle stringToColorStyle(const QString& str) const;
    QColor colorStyleToQColor(ColorStyle style) const;

    ColorStyle m_originalColorStyle;
};

#endif // COLORMENUCONTROLLER_H
