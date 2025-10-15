#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QDebug>

// Forward declarations of all our services
class OsdViewModel;
class MenuViewModel;
class MainMenuController;
class ApplicationController;

class ServiceManager : public QObject
{
    Q_OBJECT
private:
    explicit ServiceManager(QObject *parent = nullptr);
    static ServiceManager* s_instance;
    QHash<QString, QObject*> m_services;

public:
    static ServiceManager* instance();

    // Register a service with the manager
    void registerService(const QString& name, QObject* service);

    // Get a service by its type
    template <typename T>
    T* get() const {
        T* service = qobject_cast<T*>(m_services.value(T::staticMetaObject.className()));
        if (!service) {
            qWarning() << "ServiceManager: Service not found for type" << T::staticMetaObject.className();
        }
        return service;
    }
};

#endif // SERVICEMANAGER_H
