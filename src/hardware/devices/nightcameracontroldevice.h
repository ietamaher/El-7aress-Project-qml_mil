/**
 * @file nightcameracontroldevice.h
 * @brief Refactored Night Camera device following MIL-STD architecture
 *
 * @author refactored_to_milstd
 * @date 2025-10-28
 * @version 2.0
 */

#ifndef NIGHTCAMERACONTROLDEVICE_H
#define NIGHTCAMERACONTROLDEVICE_H

#include "../devices/TemplatedDevice.h"
#include "../data/DataTypes.h"
#include <QTimer>

class Transport;
class NightCameraProtocolParser;
class Message;

class NightCameraControlDevice : public TemplatedDevice<NightCameraData> {
    Q_OBJECT
public:
    explicit NightCameraControlDevice(const QString& identifier, QObject* parent = nullptr);
    ~NightCameraControlDevice() override;

    QString identifier() const { return m_identifier; }

    Q_INVOKABLE void setDependencies(Transport* transport, NightCameraProtocolParser* parser);
    Q_INVOKABLE bool initialize() override;
    void shutdown() override;
    DeviceType type() const override { return DeviceType::NightCamera; }

    // Camera controls
    Q_INVOKABLE void performFFC();
    Q_INVOKABLE void setDigitalZoom(quint8 zoomLevel);
    Q_INVOKABLE void setVideoModeLUT(quint16 mode);
    Q_INVOKABLE void getCameraStatus();

signals:
    void nightCameraDataChanged(const NightCameraData& data);

private slots:
    void processFrame(const QByteArray& frame);
    void processMessage(const Message& message);
    void checkCameraStatus();

private:
    void sendCommand(quint8 function, const QByteArray& data);

    QString m_identifier;
    Transport* m_transport = nullptr;
    NightCameraProtocolParser* m_parser = nullptr;
    QTimer* m_statusCheckTimer = nullptr;
};

#endif // NIGHTCAMERACONTROLDEVICE_H
