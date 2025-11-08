#include "systemdatalogger.h"
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QtConcurrent>

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
    m_databaseEnabled(false),
    m_databaseWriteTimer(nullptr),
    m_databaseWriteInProgress(0)
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
    m_databaseEnabled(config.enableDatabasePersistence),
    m_databaseWriteTimer(nullptr),
    m_databaseWriteInProgress(0),
    m_lastWrittenTimestamp_deviceStatus(0),
    m_lastWrittenTimestamp_gimbalMotion(0),
    m_lastWrittenTimestamp_imuData(0),
    m_lastWrittenTimestamp_trackingData(0),
    m_lastWrittenTimestamp_weaponStatus(0),
    m_lastWrittenTimestamp_cameraStatus(0),
    m_lastWrittenTimestamp_sensorData(0),
    m_lastWrittenTimestamp_ballisticData(0),
    m_lastWrittenTimestamp_userInput(0)
{
    initializeBuffers();

    if (m_databaseEnabled) {
        initializeDatabase();

        // Setup periodic database write timer (background thread)
        m_databaseWriteTimer = new QTimer(this);
        m_databaseWriteTimer->setInterval(config.databaseWriteIntervalSec * 1000);  // Convert to ms
        connect(m_databaseWriteTimer, &QTimer::timeout,
                this, &SystemDataLogger::onDatabaseWriteTimerTimeout);
        m_databaseWriteTimer->start();

        qInfo() << "SystemDataLogger: Background database writer enabled (interval:"
                << config.databaseWriteIntervalSec << "seconds)";
    }
}

SystemDataLogger::~SystemDataLogger()
{
    if (m_databaseEnabled && m_database.isOpen()) {
        // Stop timer
        if (m_databaseWriteTimer) {
            m_databaseWriteTimer->stop();
        }

        // Wait for any pending write to complete (with timeout)
        int timeout = 0;
        while (m_databaseWriteInProgress.loadAcquire() == 1 && timeout < 50) {
            QThread::msleep(100);  // Wait 100ms
            timeout++;
        }

        // Final write of any remaining data
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
               "temperature REAL,"
               "enable_stabilization INTEGER)");

    // Tracking Data table
    query.exec("CREATE TABLE IF NOT EXISTS tracking_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "tracking_phase INTEGER,"
               "tracking_active INTEGER,"
               "has_valid_target INTEGER,"
               "target_az REAL,"
               "target_el REAL,"
               "target_center_x REAL,"
               "target_center_y REAL,"
               "target_width REAL,"
               "target_height REAL,"
               "acquisition_box_x REAL,"
               "acquisition_box_y REAL,"
               "acquisition_box_w REAL,"
               "acquisition_box_h REAL)");

    // Weapon Status table
    query.exec("CREATE TABLE IF NOT EXISTS weapon_status ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "gun_armed INTEGER,"
               "ammo_loaded INTEGER,"
               "authorized INTEGER,"
               "fire_mode INTEGER,"
               "ammunition_level INTEGER,"
               "solenoid_state INTEGER,"
               "in_no_fire_zone INTEGER,"
               "in_no_traverse_zone INTEGER)");

    // Camera Status table
    query.exec("CREATE TABLE IF NOT EXISTS camera_status ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "active_camera_is_day INTEGER,"
               "day_zoom_position REAL,"
               "day_current_hfov REAL,"
               "night_zoom_position REAL,"
               "night_current_hfov REAL,"
               "image_width INTEGER,"
               "image_height INTEGER)");

    // Sensor Data table
    query.exec("CREATE TABLE IF NOT EXISTS sensor_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "lrf_distance REAL,"
               "lrf_system_status INTEGER,"
               "radar_plot_count INTEGER,"
               "selected_radar_track_id INTEGER)");

    // Ballistic Data table
    query.exec("CREATE TABLE IF NOT EXISTS ballistic_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "zeroing_mode_active INTEGER,"
               "zeroing_azimuth_offset REAL,"
               "zeroing_elevation_offset REAL,"
               "windage_mode_active INTEGER,"
               "windage_speed_knots REAL,"
               "windage_direction REAL,"
               "lead_angle_active INTEGER,"
               "lead_angle_status INTEGER,"
               "lead_angle_offset_az REAL,"
               "lead_angle_offset_el REAL,"
               "target_range REAL,"
               "target_angular_rate_az REAL,"
               "target_angular_rate_el REAL)");

    // User Input table
    query.exec("CREATE TABLE IF NOT EXISTS user_input ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp INTEGER NOT NULL,"
               "joystick_az_value REAL,"
               "joystick_el_value REAL,"
               "dead_man_switch_active INTEGER,"
               "up_track_button INTEGER,"
               "down_track_button INTEGER,"
               "menu_up INTEGER,"
               "menu_down INTEGER,"
               "menu_val INTEGER)");

    // Create indexes for faster queries on all tables
    query.exec("CREATE INDEX IF NOT EXISTS idx_device_status_timestamp ON device_status(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_gimbal_motion_timestamp ON gimbal_motion(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_imu_data_timestamp ON imu_data(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_tracking_data_timestamp ON tracking_data(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_weapon_status_timestamp ON weapon_status(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_camera_status_timestamp ON camera_status(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_sensor_data_timestamp ON sensor_data(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_ballistic_data_timestamp ON ballistic_data(timestamp)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_user_input_timestamp ON user_input(timestamp)");

    qInfo() << "SystemDataLogger: Database initialized at" << m_config.databasePath;
    qInfo() << "  Tables created: 9 (device_status, gimbal_motion, imu_data, tracking_data,";
    qInfo() << "                     weapon_status, camera_status, sensor_data, ballistic_data, user_input)";
    qInfo() << "  Indexes created: 9 (timestamp indexes for all tables)";
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

    // Note: Database writes are now handled by background timer (onDatabaseWriteTimerTimeout)
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

    int totalRecordsWritten = 0;

    // Start SQL transaction for batch write (much faster than individual inserts)
    m_database.transaction();

    try {
        // Write Device Status data
        auto deviceStatusData = m_deviceStatusBuffer.getAll();
        if (!deviceStatusData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO device_status (timestamp, az_motor_temp, az_driver_temp, "
                         "el_motor_temp, el_driver_temp, panel_temp, station_temp, station_pressure, "
                         "day_cam_connected, night_cam_connected, emergency_stop) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

            for (const auto& data : deviceStatusData) {
                if (data.timestampMs > m_lastWrittenTimestamp_deviceStatus) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(data.azMotorTemp);
                    query.addBindValue(data.azDriverTemp);
                    query.addBindValue(data.elMotorTemp);
                    query.addBindValue(data.elDriverTemp);
                    query.addBindValue(data.panelTemperature);
                    query.addBindValue(data.stationTemperature);
                    query.addBindValue(data.stationPressure);
                    query.addBindValue(data.dayCameraConnected ? 1 : 0);
                    query.addBindValue(data.nightCameraConnected ? 1 : 0);
                    query.addBindValue(data.emergencyStopActive ? 1 : 0);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_deviceStatus = data.timestampMs;
                    }
                }
            }
        }

        // Write Gimbal Motion data (sample every 10th record to reduce DB size)
        auto gimbalData = m_gimbalMotionBuffer.getAll();
        if (!gimbalData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO gimbal_motion (timestamp, gimbal_az, gimbal_el, "
                         "az_speed, el_speed, op_mode, motion_mode) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?)");

            int count = 0;
            for (const auto& data : gimbalData) {
                if (data.timestampMs > m_lastWrittenTimestamp_gimbalMotion) {
                    // Write every 10th sample (reduces 60Hz to 6Hz in database)
                    if (count++ % 10 == 0) {
                        query.addBindValue(data.timestampMs);
                        query.addBindValue(data.gimbalAz);
                        query.addBindValue(data.gimbalEl);
                        query.addBindValue(data.azimuthSpeed);
                        query.addBindValue(data.elevationSpeed);
                        query.addBindValue(static_cast<int>(data.opMode));
                        query.addBindValue(static_cast<int>(data.motionMode));

                        if (query.exec()) {
                            totalRecordsWritten++;
                        }
                    }
                    m_lastWrittenTimestamp_gimbalMotion = data.timestampMs;
                }
            }
        }

        // Write IMU data (sample every 10th record to reduce DB size)
        auto imuData = m_imuDataBuffer.getAll();
        if (!imuData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO imu_data (timestamp, roll, pitch, yaw, gyro_x, gyro_y, gyro_z, "
                         "accel_x, accel_y, accel_z, temperature, enable_stabilization) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

            int count = 0;
            for (const auto& data : imuData) {
                if (data.timestampMs > m_lastWrittenTimestamp_imuData) {
                    // Write every 10th sample (reduces 100Hz to 10Hz in database)
                    if (count++ % 10 == 0) {
                        query.addBindValue(data.timestampMs);
                        query.addBindValue(data.imuRollDeg);
                        query.addBindValue(data.imuPitchDeg);
                        query.addBindValue(data.imuYawDeg);
                        query.addBindValue(data.gyroX);
                        query.addBindValue(data.gyroY);
                        query.addBindValue(data.gyroZ);
                        query.addBindValue(data.accelX);
                        query.addBindValue(data.accelY);
                        query.addBindValue(data.accelZ);
                        query.addBindValue(data.temperature);
                        query.addBindValue(data.enableStabilization ? 1 : 0);

                        if (query.exec()) {
                            totalRecordsWritten++;
                        }
                    }
                    m_lastWrittenTimestamp_imuData = data.timestampMs;
                }
            }
        }

        // Write Tracking data
        auto trackingData = m_trackingDataBuffer.getAll();
        if (!trackingData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO tracking_data (timestamp, tracking_phase, tracking_active, "
                         "has_valid_target, target_az, target_el, target_center_x, target_center_y, "
                         "target_width, target_height, acquisition_box_x, acquisition_box_y, "
                         "acquisition_box_w, acquisition_box_h) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

            for (const auto& data : trackingData) {
                if (data.timestampMs > m_lastWrittenTimestamp_trackingData) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(static_cast<int>(data.trackingPhase));
                    query.addBindValue(data.trackingActive ? 1 : 0);
                    query.addBindValue(data.trackerHasValidTarget ? 1 : 0);
                    query.addBindValue(data.targetAz);
                    query.addBindValue(data.targetEl);
                    query.addBindValue(data.trackedTargetCenterX_px);
                    query.addBindValue(data.trackedTargetCenterY_px);
                    query.addBindValue(data.trackedTargetWidth_px);
                    query.addBindValue(data.trackedTargetHeight_px);
                    query.addBindValue(data.acquisitionBoxX_px);
                    query.addBindValue(data.acquisitionBoxY_px);
                    query.addBindValue(data.acquisitionBoxW_px);
                    query.addBindValue(data.acquisitionBoxH_px);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_trackingData = data.timestampMs;
                    }
                }
            }
        }

        // Write Weapon Status data
        auto weaponData = m_weaponStatusBuffer.getAll();
        if (!weaponData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO weapon_status (timestamp, gun_armed, ammo_loaded, authorized, "
                         "fire_mode, ammunition_level, solenoid_state, in_no_fire_zone, in_no_traverse_zone) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

            for (const auto& data : weaponData) {
                if (data.timestampMs > m_lastWrittenTimestamp_weaponStatus) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(data.gunArmed ? 1 : 0);
                    query.addBindValue(data.ammoLoaded ? 1 : 0);
                    query.addBindValue(data.authorized ? 1 : 0);
                    query.addBindValue(static_cast<int>(data.fireMode));
                    query.addBindValue(data.stationAmmunitionLevel);
                    query.addBindValue(data.solenoidState ? 1 : 0);
                    query.addBindValue(data.isReticleInNoFireZone ? 1 : 0);
                    query.addBindValue(data.isReticleInNoTraverseZone ? 1 : 0);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_weaponStatus = data.timestampMs;
                    }
                }
            }
        }

        // Write Camera Status data
        auto cameraData = m_cameraStatusBuffer.getAll();
        if (!cameraData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO camera_status (timestamp, active_camera_is_day, day_zoom_position, "
                         "day_current_hfov, night_zoom_position, night_current_hfov, image_width, image_height) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

            for (const auto& data : cameraData) {
                if (data.timestampMs > m_lastWrittenTimestamp_cameraStatus) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(data.activeCameraIsDay ? 1 : 0);
                    query.addBindValue(data.dayZoomPosition);
                    query.addBindValue(data.dayCurrentHFOV);
                    query.addBindValue(data.nightZoomPosition);
                    query.addBindValue(data.nightCurrentHFOV);
                    query.addBindValue(data.currentImageWidthPx);
                    query.addBindValue(data.currentImageHeightPx);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_cameraStatus = data.timestampMs;
                    }
                }
            }
        }

        // Write Sensor data
        auto sensorData = m_sensorDataBuffer.getAll();
        if (!sensorData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO sensor_data (timestamp, lrf_distance, lrf_system_status, "
                         "radar_plot_count, selected_radar_track_id) "
                         "VALUES (?, ?, ?, ?, ?)");

            for (const auto& data : sensorData) {
                if (data.timestampMs > m_lastWrittenTimestamp_sensorData) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(data.lrfDistance);
                    query.addBindValue(data.lrfSystemStatus);
                    query.addBindValue(data.radarPlotCount);
                    query.addBindValue(data.selectedRadarTrackId);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_sensorData = data.timestampMs;
                    }
                }
            }
        }

        // Write Ballistic data
        auto ballisticData = m_ballisticDataBuffer.getAll();
        if (!ballisticData.isEmpty()) {
            QSqlQuery query(m_database);
            query.prepare("INSERT INTO ballistic_data (timestamp, zeroing_mode_active, zeroing_azimuth_offset, "
                         "zeroing_elevation_offset, windage_mode_active, windage_speed_knots, windage_direction, "
                         "lead_angle_active, lead_angle_status, lead_angle_offset_az, lead_angle_offset_el, "
                         "target_range, target_angular_rate_az, target_angular_rate_el) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

            for (const auto& data : ballisticData) {
                if (data.timestampMs > m_lastWrittenTimestamp_ballisticData) {
                    query.addBindValue(data.timestampMs);
                    query.addBindValue(data.zeroingModeActive ? 1 : 0);
                    query.addBindValue(data.zeroingAzimuthOffset);
                    query.addBindValue(data.zeroingElevationOffset);
                    query.addBindValue(data.windageModeActive ? 1 : 0);
                    query.addBindValue(data.windageSpeedKnots);
                    query.addBindValue(data.windageDirection);
                    query.addBindValue(data.leadAngleActive ? 1 : 0);
                    query.addBindValue(static_cast<int>(data.leadAngleStatus));
                    query.addBindValue(data.leadAngleOffsetAz);
                    query.addBindValue(data.leadAngleOffsetEl);
                    query.addBindValue(data.currentTargetRange);
                    query.addBindValue(data.currentTargetAngularRateAz);
                    query.addBindValue(data.currentTargetAngularRateEl);

                    if (query.exec()) {
                        totalRecordsWritten++;
                        m_lastWrittenTimestamp_ballisticData = data.timestampMs;
                    }
                }
            }
        }

        // Commit transaction
        m_database.commit();

        qDebug() << "SystemDataLogger: Database write complete -" << totalRecordsWritten << "records written";
        emit databaseWriteComplete(totalRecordsWritten);

    } catch (...) {
        // Rollback on error
        m_database.rollback();
        qWarning() << "SystemDataLogger: Database write failed, transaction rolled back";
    }
}

void SystemDataLogger::cleanupOldData()
{
    if (!m_databaseEnabled || !m_database.isOpen()) {
        return;
    }

    // Default retention: 30 days
    qint64 retentionDays = 30;
    qint64 cutoffTimestamp = QDateTime::currentDateTime().addDays(-retentionDays).toMSecsSinceEpoch();

    QSqlQuery query(m_database);

    // Delete old data from all tables
    query.exec(QString("DELETE FROM device_status WHERE timestamp < %1").arg(cutoffTimestamp));
    int deviceDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM gimbal_motion WHERE timestamp < %1").arg(cutoffTimestamp));
    int gimbalDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM imu_data WHERE timestamp < %1").arg(cutoffTimestamp));
    int imuDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM tracking_data WHERE timestamp < %1").arg(cutoffTimestamp));
    int trackingDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM weapon_status WHERE timestamp < %1").arg(cutoffTimestamp));
    int weaponDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM camera_status WHERE timestamp < %1").arg(cutoffTimestamp));
    int cameraDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM sensor_data WHERE timestamp < %1").arg(cutoffTimestamp));
    int sensorDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM ballistic_data WHERE timestamp < %1").arg(cutoffTimestamp));
    int ballisticDeleted = query.numRowsAffected();

    query.exec(QString("DELETE FROM user_input WHERE timestamp < %1").arg(cutoffTimestamp));
    int userInputDeleted = query.numRowsAffected();

    int totalDeleted = deviceDeleted + gimbalDeleted + imuDeleted + trackingDeleted +
                       weaponDeleted + cameraDeleted + sensorDeleted + ballisticDeleted +
                       userInputDeleted;

    if (totalDeleted > 0) {
        // Vacuum database to reclaim space
        query.exec("VACUUM");

        qInfo() << "SystemDataLogger: Cleaned up" << totalDeleted << "old records (older than"
                << retentionDays << "days)";
    }
}

// ============================================================================
// BACKGROUND DATABASE WRITER
// ============================================================================

void SystemDataLogger::onDatabaseWriteTimerTimeout()
{
    if (!m_databaseEnabled || !m_database.isOpen()) {
        return;
    }

    // Check if a write is already in progress (skip if busy)
    if (m_databaseWriteInProgress.loadAcquire() == 1) {
        qDebug() << "SystemDataLogger: Skipping database write - previous write still in progress";
        return;
    }

    // Mark write as in progress
    m_databaseWriteInProgress.storeRelease(1);

    // Launch background thread for database write
    // Note: We capture 'this' to call writePendingDataToDatabase, which is thread-safe
    // because all ring buffers have mutex protection
    QFuture<void> future = QtConcurrent::run([this]() {
        try {
            writePendingDataToDatabase();
        } catch (const std::exception& e) {
            qWarning() << "SystemDataLogger: Exception in background write:" << e.what();
        } catch (...) {
            qWarning() << "SystemDataLogger: Unknown exception in background write";
        }

        // Mark write as complete
        m_databaseWriteInProgress.storeRelease(0);
    });

    qDebug() << "SystemDataLogger: Database write started on background thread";
}

void SystemDataLogger::onBackgroundWriteFinished(int recordsWritten)
{
    qDebug() << "SystemDataLogger: Background write completed -" << recordsWritten << "records written";
    m_databaseWriteInProgress.storeRelease(0);
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
