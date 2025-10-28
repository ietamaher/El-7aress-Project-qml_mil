#include "systemdatalogger.h"
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

SystemDataLogger::SystemDataLogger(QObject *parent)
    : QObject(parent),
    m_deviceStatusBuffer(3600),
    m_gimbalMotionBuffer(36000),
    m_imuDataBuffer(60000),
    m_trackingDataBuffer(18000),
    m_weaponStatusBuffer(3600),
    m_cameraStatusBuffer(1800),
    m_sensorDataBuffer(6000),
    m_ballisticDataBuffer(1800),
    m_userInputBuffer(6000),
    m_databaseEnabled(false)
{
    initializeBuffers();
}

SystemDataLogger::SystemDataLogger(const LoggerConfig& config, QObject *parent)
    : QObject(parent),
    m_deviceStatusBuffer(config.deviceStatusBufferSize),
    m_gimbalMotionBuffer(config.gimbalMotionBufferSize),
    m_imuDataBuffer(config.imuDataBufferSize),
    m_trackingDataBuffer(config.trackingDataBufferSize),
    m_weaponStatusBuffer(config.weaponStatusBufferSize),
    m_cameraStatusBuffer(config.cameraStatusBufferSize),
    m_sensorDataBuffer(config.sensorDataBufferSize),
    m_ballisticDataBuffer(config.ballisticDataBufferSize),
    m_userInputBuffer(config.userInputBufferSize),
    m_config(config),
    m_databaseEnabled(config.enableDatabasePersistence)
{
    initializeBuffers();

    if (m_databaseEnabled) {
        initializeDatabase();
    }
}

SystemDataLogger::~SystemDataLogger()
{
    if (m_databaseEnabled && m_database.isOpen()) {
        writePendingDataToDatabase();
        m_database.close();
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void SystemDataLogger::initializeBuffers()
{
    qInfo() << "SystemDataLogger: Initializing buffers";
    qInfo() << "  Device Status:" << m_deviceStatusBuffer.size() << "samples";
    qInfo() << "  Gimbal Motion:" << m_gimbalMotionBuffer.size() << "samples";
    qInfo() << "  IMU Data:" << m_imuDataBuffer.size() << "samples";
    qInfo() << "  Tracking Data:" << m_trackingDataBuffer.size() << "samples";

    m_lastDbWrite = QDateTime::currentDateTime();
}

void SystemDataLogger::initializeDatabase()
{
    // Initialize SQLite database for long-term storage
    m_database = QSqlDatabase::addDatabase("QSQLITE", "rcws_logger");
    m_database.setDatabaseName(m_config.databasePath);

    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        m_databaseEnabled = false;
        return;
    }

    // Create tables for each category
    QSqlQuery query(m_database);

    // Device Status table
    query.exec("CREATE TABLE IF NOT EXISTS device_status ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "az_motor_temp REAL,"
               "az_driver_temp REAL,"
               "el_motor_temp REAL,"
               "el_driver_temp REAL,"
               "panel_temp REAL,"
               "station_temp REAL,"
               "station_pressure REAL,"
               "day_cam_connected INTEGER,"
               "night_cam_connected INTEGER,"
               "emergency_stop INTEGER)");

    // Gimbal Motion table
    query.exec("CREATE TABLE IF NOT EXISTS gimbal_motion ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "gimbal_az REAL,"
               "gimbal_el REAL,"
               "az_speed REAL,"
               "el_speed REAL,"
               "op_mode INTEGER,"
               "motion_mode INTEGER)");

    // IMU Data table
    query.exec("CREATE TABLE IF NOT EXISTS imu_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "roll REAL,"
               "pitch REAL,"
               "yaw REAL,"
               "gyro_x REAL,"
               "gyro_y REAL,"
               "gyro_z REAL,"
               "accel_x REAL,"
               "accel_y REAL,"
               "accel_z REAL,"
               "temperature REAL)");

    // Create indexes for faster queries
    query.exec("CREATE INDEX IF NOT EXISTS idx_device_status_timestamp ON device_status(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_gimbal_motion_timestamp ON gimbal_motion(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_imu_data_timestamp ON imu_data(timestamp)");

    qInfo() << "SystemDataLogger: Database initialized at" << m_config.databasePath;
}

// ============================================================================
// MAIN DATA LOGGING SLOT
// ============================================================================

void SystemDataLogger::onSystemStateChanged(const SystemStateData& state)
{
    // Extract data into appropriate categories
    QDateTime now = QDateTime::currentDateTime();

    // Log device status (lower frequency)
    static QDateTime lastDeviceStatusLog;
    if (lastDeviceStatusLog.isNull() ||
        lastDeviceStatusLog.msecsTo(now) >= 1000) {  // 1 Hz

        DeviceStatusData deviceStatus = extractDeviceStatus(state);
        deviceStatus.timestamp = now;
        deviceStatus.timestampMs = now.toMSecsSinceEpoch();
        m_deviceStatusBuffer.append(deviceStatus);
        emit dataLogged(DataCategory::DeviceStatus, now);
        lastDeviceStatusLog = now;
    }

    // Log gimbal motion (higher frequency)
    GimbalMotionData gimbalMotion = extractGimbalMotion(state);
    gimbalMotion.timestamp = now;
    gimbalMotion.timestampMs = now.toMSecsSinceEpoch();
    m_gimbalMotionBuffer.append(gimbalMotion);
    emit dataLogged(DataCategory::GimbalMotion, now);

    // Log IMU data (highest frequency)
    ImuDataPoint imuData = extractImuData(state);
    imuData.timestamp = now;
    imuData.timestampMs = now.toMSecsSinceEpoch();
    m_imuDataBuffer.append(imuData);
    emit dataLogged(DataCategory::ImuData, now);

    // Log tracking data
    static QDateTime lastTrackingLog;
    if (lastTrackingLog.isNull() ||
        lastTrackingLog.msecsTo(now) >= 33) {  // ~30 Hz

        TrackingDataPoint trackingData = extractTrackingData(state);
        trackingData.timestamp = now;
        trackingData.timestampMs = now.toMSecsSinceEpoch();
        m_trackingDataBuffer.append(trackingData);
        emit dataLogged(DataCategory::TrackingData, now);
        lastTrackingLog = now;
    }

    // Log weapon status
    static QDateTime lastWeaponLog;
    if (lastWeaponLog.isNull() ||
        lastWeaponLog.msecsTo(now) >= 1000) {  // 1 Hz

        WeaponStatusData weaponStatus = extractWeaponStatus(state);
        weaponStatus.timestamp = now;
        weaponStatus.timestampMs = now.toMSecsSinceEpoch();
        m_weaponStatusBuffer.append(weaponStatus);
        emit dataLogged(DataCategory::WeaponStatus, now);
        lastWeaponLog = now;
    }

    // Log camera status
    static QDateTime lastCameraLog;
    if (lastCameraLog.isNull() ||
        lastCameraLog.msecsTo(now) >= 1000) {  // 1 Hz

        CameraStatusData cameraStatus = extractCameraStatus(state);
        cameraStatus.timestamp = now;
        cameraStatus.timestampMs = now.toMSecsSinceEpoch();
        m_cameraStatusBuffer.append(cameraStatus);
        emit dataLogged(DataCategory::CameraStatus, now);
        lastCameraLog = now;
    }

    // Log sensor data
    static QDateTime lastSensorLog;
    if (lastSensorLog.isNull() ||
        lastSensorLog.msecsTo(now) >= 100) {  // 10 Hz

        SensorDataPoint sensorData = extractSensorData(state);
        sensorData.timestamp = now;
        sensorData.timestampMs = now.toMSecsSinceEpoch();
        m_sensorDataBuffer.append(sensorData);
        emit dataLogged(DataCategory::SensorData, now);
        lastSensorLog = now;
    }

    // Log ballistic data
    static QDateTime lastBallisticLog;
    if (lastBallisticLog.isNull() ||
        lastBallisticLog.msecsTo(now) >= 1000) {  // 1 Hz

        BallisticDataPoint ballisticData = extractBallisticData(state);
        ballisticData.timestamp = now;
        ballisticData.timestampMs = now.toMSecsSinceEpoch();
        m_ballisticDataBuffer.append(ballisticData);
        emit dataLogged(DataCategory::BallisticData, now);
        lastBallisticLog = now;
    }

    // Log user input
    static QDateTime lastInputLog;
    if (lastInputLog.isNull() ||
        lastInputLog.msecsTo(now) >= 100) {  // 10 Hz

        UserInputData userInput = extractUserInput(state);
        userInput.timestamp = now;
        userInput.timestampMs = now.toMSecsSinceEpoch();
        m_userInputBuffer.append(userInput);
        emit dataLogged(DataCategory::UserInput, now);
        lastInputLog = now;
    }

    // Periodically write to database if enabled
    if (m_databaseEnabled &&
        m_lastDbWrite.secsTo(now) >= m_config.databaseWriteIntervalSec) {
        writePendingDataToDatabase();
        m_lastDbWrite = now;
    }
}

// ============================================================================
// DATA EXTRACTION METHODS
// ============================================================================

DeviceStatusData SystemDataLogger::extractDeviceStatus(const SystemStateData& state)
{
    DeviceStatusData data;
    data.azMotorTemp = state.azMotorTemp;
    data.azDriverTemp = state.azDriverTemp;
    data.elMotorTemp = state.elMotorTemp;
    data.elDriverTemp = state.elDriverTemp;
    data.panelTemperature = state.panelTemperature;
    data.stationTemperature = state.stationTemperature;
    data.stationPressure = state.stationPressure;
    data.dayCameraConnected = state.dayCameraConnected;
    data.nightCameraConnected = state.nightCameraConnected;
    data.dayCameraError = state.dayCameraError;
    data.nightCameraError = state.nightCameraError;
    data.emergencyStopActive = state.emergencyStopActive;
    data.stationEnabled = state.stationEnabled;
    return data;
}

GimbalMotionData SystemDataLogger::extractGimbalMotion(const SystemStateData& state)
{
    GimbalMotionData data;
    data.gimbalAz = state.gimbalAz;
    data.gimbalEl = state.gimbalEl;
    data.azimuthSpeed = state.azimuthSpeed;
    data.elevationSpeed = state.elevationSpeed;
    data.azimuthDirection = state.azimuthDirection;
    data.elevationDirection = state.elevationDirection;
    data.gimbalSpeed = state.gimbalSpeed;
    data.actuatorPosition = state.actuatorPosition;
    data.opMode = state.opMode;
    data.motionMode = state.motionMode;
    return data;
}

ImuDataPoint SystemDataLogger::extractImuData(const SystemStateData& state)
{
    ImuDataPoint data;
    data.imuRollDeg = state.imuRollDeg;
    data.imuPitchDeg = state.imuPitchDeg;
    data.imuYawDeg = state.imuYawDeg;
    data.gyroX = state.GyroX;
    data.gyroY = state.GyroY;
    data.gyroZ = state.GyroZ;
    data.accelX = state.AccelX;
    data.accelY = state.AccelY;
    data.accelZ = state.AccelZ;
    data.temperature = state.temperature;
    data.enableStabilization = state.enableStabilization;
    return data;
}

TrackingDataPoint SystemDataLogger::extractTrackingData(const SystemStateData& state)
{
    TrackingDataPoint data;
    data.trackingPhase = state.currentTrackingPhase;
    data.trackerHasValidTarget = state.trackerHasValidTarget;
    data.trackingActive = state.trackingActive;
    data.acquisitionBoxX_px = state.acquisitionBoxX_px;
    data.acquisitionBoxY_px = state.acquisitionBoxY_px;
    data.acquisitionBoxW_px = state.acquisitionBoxW_px;
    data.acquisitionBoxH_px = state.acquisitionBoxH_px;
    data.trackedTargetCenterX_px = state.trackedTargetCenterX_px;
    data.trackedTargetCenterY_px = state.trackedTargetCenterY_px;
    data.trackedTargetWidth_px = state.trackedTargetWidth_px;
    data.trackedTargetHeight_px = state.trackedTargetHeight_px;
    data.targetAz = state.targetAz;
    data.targetEl = state.targetEl;
    return data;
}

WeaponStatusData SystemDataLogger::extractWeaponStatus(const SystemStateData& state)
{
    WeaponStatusData data;
    data.gunArmed = state.gunArmed;
    data.ammoLoaded = state.ammoLoaded;
    data.authorized = state.authorized;
    data.deadManSwitchActive = state.deadManSwitchActive;
    data.detectionEnabled = state.detectionEnabled;
    data.fireMode = state.fireMode;
    data.stationAmmunitionLevel = state.stationAmmunitionLevel;
    data.solenoidState = state.solenoidState;
    data.isReticleInNoFireZone = state.isReticleInNoFireZone;
    data.isReticleInNoTraverseZone = state.isReticleInNoTraverseZone;
    return data;
}

CameraStatusData SystemDataLogger::extractCameraStatus(const SystemStateData& state)
{
    CameraStatusData data;
    data.activeCameraIsDay = state.activeCameraIsDay;
    data.dayZoomPosition = state.dayZoomPosition;
    data.dayCurrentHFOV = state.dayCurrentHFOV;
    data.nightZoomPosition = state.nightZoomPosition;
    data.nightCurrentHFOV = state.nightCurrentHFOV;
    data.currentImageWidthPx = state.currentImageWidthPx;
    data.currentImageHeightPx = state.currentImageHeightPx;
    return data;
}

SensorDataPoint SystemDataLogger::extractSensorData(const SystemStateData& state)
{
    SensorDataPoint data;
    data.lrfDistance = state.lrfDistance;
    data.lrfSystemStatus = state.lrfSystemStatus;
    data.radarPlotCount = state.radarPlots.size();
    data.selectedRadarTrackId = state.selectedRadarTrackId;
    return data;
}

BallisticDataPoint SystemDataLogger::extractBallisticData(const SystemStateData& state)
{
    BallisticDataPoint data;
    data.zeroingModeActive = state.zeroingModeActive;
    data.zeroingAzimuthOffset = state.zeroingAzimuthOffset;
    data.zeroingElevationOffset = state.zeroingElevationOffset;
    data.windageModeActive = state.windageModeActive;
    data.windageSpeedKnots = state.windageSpeedKnots;
    data.windageDirection = state.windageDirectionDegrees;
    data.leadAngleActive = state.leadAngleCompensationActive;
    data.leadAngleStatus = state.currentLeadAngleStatus;
    data.leadAngleOffsetAz = state.leadAngleOffsetAz;
    data.leadAngleOffsetEl = state.leadAngleOffsetEl;
    data.currentTargetRange = state.currentTargetRange;
    data.currentTargetAngularRateAz = state.currentTargetAngularRateAz;
    data.currentTargetAngularRateEl = state.currentTargetAngularRateEl;
    return data;
}

UserInputData SystemDataLogger::extractUserInput(const SystemStateData& state)
{
    UserInputData data;
    data.joystickAzValue = state.joystickAzValue;
    data.joystickElValue = state.joystickElValue;
    data.deadManSwitchActive = state.deadManSwitchActive;
    data.upTrackButton = state.upTrackButton;
    data.downTrackButton = state.downTrackButton;
    data.menuUp = state.menuUp;
    data.menuDown = state.menuDown;
    data.menuVal = state.menuVal;
    return data;
}

// ============================================================================
// QUERY METHODS
// ============================================================================

QVector<DeviceStatusData> SystemDataLogger::getDeviceStatusHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_deviceStatusBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<GimbalMotionData> SystemDataLogger::getGimbalMotionHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_gimbalMotionBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<ImuDataPoint> SystemDataLogger::getImuHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_imuDataBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<TrackingDataPoint> SystemDataLogger::getTrackingHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_trackingDataBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<WeaponStatusData> SystemDataLogger::getWeaponStatusHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_weaponStatusBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<CameraStatusData> SystemDataLogger::getCameraStatusHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_cameraStatusBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<SensorDataPoint> SystemDataLogger::getSensorHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_sensorDataBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<BallisticDataPoint> SystemDataLogger::getBallisticHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_ballisticDataBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

QVector<UserInputData> SystemDataLogger::getUserInputHistory(
    const QDateTime& startTime, const QDateTime& endTime) const
{
    return m_userInputBuffer.getRange(
        startTime.toMSecsSinceEpoch(),
        endTime.toMSecsSinceEpoch());
}

// ============================================================================
// STATISTICS AND MANAGEMENT
// ============================================================================

QPair<QDateTime, QDateTime> SystemDataLogger::getDataTimeRange(DataCategory category) const
{
    QPair<qint64, qint64> range;

    switch (category) {
    case DataCategory::DeviceStatus:
        range = m_deviceStatusBuffer.getTimeRange();
        break;
    case DataCategory::GimbalMotion:
        range = m_gimbalMotionBuffer.getTimeRange();
        break;
    case DataCategory::ImuData:
        range = m_imuDataBuffer.getTimeRange();
        break;
    case DataCategory::TrackingData:
        range = m_trackingDataBuffer.getTimeRange();
        break;
    case DataCategory::WeaponStatus:
        range = m_weaponStatusBuffer.getTimeRange();
        break;
    case DataCategory::CameraStatus:
        range = m_cameraStatusBuffer.getTimeRange();
        break;
    case DataCategory::SensorData:
        range = m_sensorDataBuffer.getTimeRange();
        break;
    case DataCategory::BallisticData:
        range = m_ballisticDataBuffer.getTimeRange();
        break;
    case DataCategory::UserInput:
        range = m_userInputBuffer.getTimeRange();
        break;
    }

    return {QDateTime::fromMSecsSinceEpoch(range.first),
            QDateTime::fromMSecsSinceEpoch(range.second)};
}

int SystemDataLogger::getSampleCount(DataCategory category) const
{
    switch (category) {
    case DataCategory::DeviceStatus:
        return m_deviceStatusBuffer.size();
    case DataCategory::GimbalMotion:
        return m_gimbalMotionBuffer.size();
    case DataCategory::ImuData:
        return m_imuDataBuffer.size();
    case DataCategory::TrackingData:
        return m_trackingDataBuffer.size();
    case DataCategory::WeaponStatus:
        return m_weaponStatusBuffer.size();
    case DataCategory::CameraStatus:
        return m_cameraStatusBuffer.size();
    case DataCategory::SensorData:
        return m_sensorDataBuffer.size();
    case DataCategory::BallisticData:
        return m_ballisticDataBuffer.size();
    case DataCategory::UserInput:
        return m_userInputBuffer.size();
    default:
        return 0;
    }
}

SystemDataLogger::MemoryStats SystemDataLogger::getMemoryUsage() const
{
    MemoryStats stats;
    stats.deviceStatusBytes = m_deviceStatusBuffer.size() * sizeof(DeviceStatusData);
    stats.gimbalMotionBytes = m_gimbalMotionBuffer.size() * sizeof(GimbalMotionData);
    stats.imuDataBytes = m_imuDataBuffer.size() * sizeof(ImuDataPoint);
    stats.trackingDataBytes = m_trackingDataBuffer.size() * sizeof(TrackingDataPoint);
    stats.weaponStatusBytes = m_weaponStatusBuffer.size() * sizeof(WeaponStatusData);
    stats.cameraStatusBytes = m_cameraStatusBuffer.size() * sizeof(CameraStatusData);
    stats.sensorDataBytes = m_sensorDataBuffer.size() * sizeof(SensorDataPoint);
    stats.ballisticDataBytes = m_ballisticDataBuffer.size() * sizeof(BallisticDataPoint);
    stats.userInputBytes = m_userInputBuffer.size() * sizeof(UserInputData);

    stats.totalBytes = stats.deviceStatusBytes + stats.gimbalMotionBytes +
                       stats.imuDataBytes + stats.trackingDataBytes +
                       stats.weaponStatusBytes + stats.cameraStatusBytes +
                       stats.sensorDataBytes + stats.ballisticDataBytes +
                       stats.userInputBytes;

    return stats;
}

void SystemDataLogger::clearAllData()
{
    m_deviceStatusBuffer.clear();
    m_gimbalMotionBuffer.clear();
    m_imuDataBuffer.clear();
    m_trackingDataBuffer.clear();
    m_weaponStatusBuffer.clear();
    m_cameraStatusBuffer.clear();
    m_sensorDataBuffer.clear();
    m_ballisticDataBuffer.clear();
    m_userInputBuffer.clear();

    qInfo() << "SystemDataLogger: All data cleared";
}

void SystemDataLogger::clearCategory(DataCategory category)
{
    switch (category) {
    case DataCategory::DeviceStatus:
        m_deviceStatusBuffer.clear();
        break;
    case DataCategory::GimbalMotion:
        m_gimbalMotionBuffer.clear();
        break;
    case DataCategory::ImuData:
        m_imuDataBuffer.clear();
        break;
    case DataCategory::TrackingData:
        m_trackingDataBuffer.clear();
        break;
    case DataCategory::WeaponStatus:
        m_weaponStatusBuffer.clear();
        break;
    case DataCategory::CameraStatus:
        m_cameraStatusBuffer.clear();
        break;
    case DataCategory::SensorData:
        m_sensorDataBuffer.clear();
        break;
    case DataCategory::BallisticData:
        m_ballisticDataBuffer.clear();
        break;
    case DataCategory::UserInput:
        m_userInputBuffer.clear();
        break;
    }
}

// ============================================================================
// DATABASE PERSISTENCE
// ============================================================================

void SystemDataLogger::setDatabasePersistence(bool enabled)
{
    if (enabled && !m_databaseEnabled) {
        initializeDatabase();
    }
    m_databaseEnabled = enabled;
}

void SystemDataLogger::writePendingDataToDatabase()
{
    if (!m_databaseEnabled || !m_database.isOpen()) {
        return;
    }

    // This is a simplified version - in production you'd batch write recent samples
    qDebug() << "SystemDataLogger: Writing data to database";
    emit databaseWriteComplete(0);
}

// ============================================================================
// CSV EXPORT
// ============================================================================

bool SystemDataLogger::exportToCSV(DataCategory category, const QString& filePath,
                                   const QDateTime& startTime, const QDateTime& endTime) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open CSV file:" << filePath;
        return false;
    }

    QTextStream out(&file);

    // Export based on category
    switch (category) {
    case DataCategory::GimbalMotion: {
        auto data = getGimbalMotionHistory(startTime, endTime);
        out << "Timestamp,GimbalAz,GimbalEl,AzSpeed,ElSpeed,OpMode,MotionMode\n";
        for (const auto& point : data) {
            out << point.timestamp.toString(Qt::ISODate) << ","
                << point.gimbalAz << ","
                << point.gimbalEl << ","
                << point.azimuthSpeed << ","
                << point.elevationSpeed << ","
                << static_cast<int>(point.opMode) << ","
                << static_cast<int>(point.motionMode) << "\n";
        }
        break;
    }
    case DataCategory::ImuData: {
        auto data = getImuHistory(startTime, endTime);
        out << "Timestamp,Roll,Pitch,Yaw,GyroX,GyroY,GyroZ,AccelX,AccelY,AccelZ\n";
        for (const auto& point : data) {
            out << point.timestamp.toString(Qt::ISODate) << ","
                << point.imuRollDeg << ","
                << point.imuPitchDeg << ","
                << point.imuYawDeg << ","
                << point.gyroX << ","
                << point.gyroY << ","
                << point.gyroZ << ","
                << point.accelX << ","
                << point.accelY << ","
                << point.accelZ << "\n";
        }
        break;
    }
    // Add other categories as needed
    default:
        qWarning() << "CSV export not implemented for this category";
        return false;
    }

    file.close();
    qInfo() << "Data exported to CSV:" << filePath;
    return true;
}
