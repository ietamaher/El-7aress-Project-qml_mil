#include "servicemanager.h"
#include <QCoreApplication>

ServiceManager* ServiceManager::s_instance = nullptr;

ServiceManager::ServiceManager(QObject *parent) : QObject(parent) {}

ServiceManager* ServiceManager::instance() {
    if (!s_instance) {
        s_instance = new ServiceManager(QCoreApplication::instance());
    }
    return s_instance;
}

void ServiceManager::registerService(const QString& name, QObject* service) {
    if (m_services.contains(name)) {
        qWarning() << "ServiceManager: Service already registered with name" << name;
        return;
    }
    m_services.insert(name, service);
}
