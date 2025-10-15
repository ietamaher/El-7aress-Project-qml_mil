#ifndef MENUVIEWMODEL_H
#define MENUVIEWMODEL_H

#include <QObject>
#include <QStringListModel>

class MenuViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QStringListModel* optionsModel READ optionsModel CONSTANT)

public:
    explicit MenuViewModel(QObject *parent = nullptr);

    bool visible() const { return m_visible; }
    QString title() const { return m_title; }
    QString description() const { return m_description; }
    int currentIndex() const { return m_currentIndex; }
    QStringListModel* optionsModel() { return &m_optionsModel; }

public slots:
    void showMenu(const QString& title, const QString& description, const QStringList& options);
    void hideMenu();
    void moveSelectionUp();
    void moveSelectionDown();
    void selectCurrentItem();
    void setCurrentIndex(int index); // NEW: Set selection programmatically

signals:
    void visibleChanged();
    void titleChanged();
    void descriptionChanged();
    void currentIndexChanged();
    void optionSelected(const QString& option);

private:
    bool m_visible = false;
    QString m_title;
    QString m_description;
    int m_currentIndex = -1;
    QStringListModel m_optionsModel;

    int findNextSelectable(int start, int direction);
    bool isSelectable(int index) const;
};

#endif // MENUVIEWMODEL_H
