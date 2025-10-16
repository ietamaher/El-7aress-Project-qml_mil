#include "servicemanager.h"

ServiceManager* ServiceManager::s_instance = nullptr;

ServiceManager* ServiceManager::instance()
{
    if (!s_instance) {
        s_instance = new ServiceManager();
    }
    return s_instance;
}

void ServiceManager::registerService(const QString& name, QObject* service)
{
    m_services.insert(name, service);
}
