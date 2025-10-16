#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class ServiceManager
{
public:
    static ServiceManager* instance();

    // Register a service with automatic type name
    void registerService(const QString& name, QObject* service);

    // Get service by type (original method)
    template<typename T>
    T* get() {
        return static_cast<T*>(m_services.value(T::staticMetaObject.className()));
    }

    // NEW: Get service by custom name
    template<typename T>
    T* get(const QString& name) {
        return static_cast<T*>(m_services.value(name));
    }

private:
    ServiceManager() = default;
    static ServiceManager* s_instance;
    QMap<QString, QObject*> m_services;
};

#endif // SERVICEMANAGER_H
