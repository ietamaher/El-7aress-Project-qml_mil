#ifndef SYSTEMDATALOGGER_H
#define SYSTEMDATALOGGER_H

/**
 * @file SystemDataLogger.h
 * @brief Time-series data logging system for RCWS with category-based storage
 *
 * This class provides efficient time-series data storage and retrieval for the RCWS system.
 * It organizes data into logical categories and maintains circular buffers for each category
 * to prevent unbounded memory growth while preserving recent historical data.
 *
 * FEATURES:
 * • Category-based data organization (device status, motion, tracking, etc.)
 * • Configurable ring buffer sizes per category
 * • Automatic timestamp management
 * • Efficient time-range queries
 * • Optional SQLite persistence for long-term storage
 * • Thread-safe operations
 * • Minimal performance impact on real-time operations
 *
 * USAGE:
 * 1. Create logger instance
 * 2. Connect to SystemStateModel signals
 * 3. Query historical data by category and time range
 * 4. Optionally enable database persistence
 *
 * @author Senior Developer
 * @date 2025
 */

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QMutex>
#include <QSqlDatabase>
#include <QTimer>
#include <QAtomicInt>
#include "models/domain/systemstatedata.h"

// ============================================================================
// DATA CATEGORIES - Organize data by logical groups
// ============================================================================

/**
 * @brief Categories for organizing different types of system data
 */
enum class DataCategory {
    DeviceStatus,      // Temperature, connection status, errors
    GimbalMotion,      // Az/El position, speed, motor temps
    ImuData,           // Roll, pitch, yaw, gyro, accel
    TrackingData,      // Tracking phase, target position, lock status
    WeaponStatus,      // Armed status, ammo, fire mode
    CameraStatus,      // Zoom, FOV, active camera
    SensorData,        // LRF, radar plots
    BallisticData,     // Zeroing, windage, lead angle
    ZoneData,          // No-fire zones, scan zones
    UserInput          // Joystick, buttons, manual controls
};

// ============================================================================
// TIME-STAMPED DATA STRUCTURES
// ============================================================================

/**
 * @brief Base structure for all timestamped data entries
 */
struct TimeStampedDataPoint {
    QDateTime timestamp;
    qint64 timestampMs; // For faster comparisons

    TimeStampedDataPoint() : timestampMs(0) {}
    explicit TimeStampedDataPoint(const QDateTime& ts)
        : timestamp(ts), timestampMs(ts.toMSecsSinceEpoch()) {}
};

/**
 * @brief Device status data point (temperatures, health, connections)
 */
struct DeviceStatusData : public TimeStampedDataPoint {
    // Motor temperatures
    float azMotorTemp = 0.0f;
    float azDriverTemp = 0.0f;
    float elMotorTemp = 0.0f;
    float elDriverTemp = 0.0f;

    // Station status
    float panelTemperature = 0.0f;
    float stationTemperature = 0.0f;
    float stationPressure = 0.0f;

    // Camera connections
    bool dayCameraConnected = false;
    bool nightCameraConnected = false;
    bool dayCameraError = false;
    bool nightCameraError = false;

    // System health
    bool emergencyStopActive = false;
    bool stationEnabled = false;
};

/**
 * @brief Gimbal motion data point (position, speed, direction)
 */
struct GimbalMotionData : public TimeStampedDataPoint {
    float gimbalAz = 0.0f;
    float gimbalEl = 0.0f;
    float azimuthSpeed = 0.0f;
    float elevationSpeed = 0.0f;
    int azimuthDirection = 0;
    int elevationDirection = 0;
    float gimbalSpeed = 0.0f;
    float actuatorPosition = 0.0f;

    // Operational context
    OperationalMode opMode = OperationalMode::Idle;
    MotionMode motionMode = MotionMode::Idle;
};

/**
 * @brief IMU sensor data point (orientation, gyro, accelerometer)
 */
struct ImuDataPoint : public TimeStampedDataPoint {
    float imuRollDeg = 0.0f;
    float imuPitchDeg = 0.0f;
    float imuYawDeg = 0.0f;

    double gyroX = 0.0;
    double gyroY = 0.0;
    double gyroZ = 0.0;

    double accelX = 0.0;
    double accelY = 0.0;
    double accelZ = 0.0;

    float temperature = 0.0f;
    bool enableStabilization = false;
};

/**
 * @brief Tracking system data point (phase, target info, box position)
 */
struct TrackingDataPoint : public TimeStampedDataPoint {
    TrackingPhase trackingPhase = TrackingPhase::Off;
    bool trackerHasValidTarget = false;
    bool trackingActive = false;

    // Acquisition box
    float acquisitionBoxX_px = 0.0f;
    float acquisitionBoxY_px = 0.0f;
    float acquisitionBoxW_px = 0.0f;
    float acquisitionBoxH_px = 0.0f;

    // Target position
    float trackedTargetCenterX_px = 0.0f;
    float trackedTargetCenterY_px = 0.0f;
    float trackedTargetWidth_px = 0.0f;
    float trackedTargetHeight_px = 0.0f;

    // Target angles
    float targetAz = 0.0f;
    float targetEl = 0.0f;
};

/**
 * @brief Weapon system data point (arming, ammo, fire control)
 */
struct WeaponStatusData : public TimeStampedDataPoint {
    bool gunArmed = false;
    bool ammoLoaded = false;
    bool authorized = false;
    bool deadManSwitchActive = false;
    bool detectionEnabled = false;

    FireMode fireMode = FireMode::Unknown;
    int stationAmmunitionLevel = 0;
    bool solenoidState = false;

    // Safety zones
    bool isReticleInNoFireZone = false;
    bool isReticleInNoTraverseZone = false;
};

/**
 * @brief Camera system data point (zoom, FOV, active camera)
 */
struct CameraStatusData : public TimeStampedDataPoint {
    bool activeCameraIsDay = true;

    // Day camera
    float dayZoomPosition = 0.0f;
    float dayCurrentHFOV = 0.0f;

    // Night camera
    float nightZoomPosition = 0.0f;
    float nightCurrentHFOV = 0.0f;

    // Image dimensions
    int currentImageWidthPx = 0;
    int currentImageHeightPx = 0;
};

/**
 * @brief Sensor data point (LRF, radar)
 */
struct SensorDataPoint : public TimeStampedDataPoint {
    float lrfDistance = 0.0f;
    quint8 lrfSystemStatus;

    // Radar plots (only IDs and counts for efficiency)
    int radarPlotCount = 0;
    int selectedRadarTrackId = 0;
};

/**
 * @brief Ballistic compensation data point (zeroing, windage, lead)
 */
struct BallisticDataPoint : public TimeStampedDataPoint {
    // Zeroing
    bool zeroingModeActive = false;
    float zeroingAzimuthOffset = 0.0f;
    float zeroingElevationOffset = 0.0f;

    // Windage
    bool windageModeActive = false;
    float windageSpeedKnots = 0.0f;
    float windageDirection = 0.0f;

    // Lead angle
    bool leadAngleActive = false;
    LeadAngleStatus leadAngleStatus = LeadAngleStatus::Off;
    float leadAngleOffsetAz = 0.0f;
    float leadAngleOffsetEl = 0.0f;

    // Target data for lead
    float currentTargetRange = 0.0f;
    float currentTargetAngularRateAz = 0.0f;
    float currentTargetAngularRateEl = 0.0f;
};

/**
 * @brief User input data point (joystick, buttons)
 */
struct UserInputData : public TimeStampedDataPoint {
    float joystickAzValue = 0.0f;
    float joystickElValue = 0.0f;

    bool deadManSwitchActive = false;
    bool upTrackButton = false;
    bool downTrackButton = false;
    bool menuUp = false;
    bool menuDown = false;
    bool menuVal = false;
};

// ============================================================================
// MAIN DATA LOGGER CLASS
// ============================================================================

/**
 * @brief Main data logger class for time-series data management
 *
 * This class provides centralized logging of all system data organized by category.
 * Each category maintains its own circular buffer with configurable size.
 */
class SystemDataLogger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Configuration for data logging behavior
     */
    struct LoggerConfig {
        // Buffer sizes (number of samples to keep in memory)
        int deviceStatusBufferSize = 3600;     // 1 hour at 1 Hz
        int gimbalMotionBufferSize = 36000;    // 10 minutes at 60 Hz
        int imuDataBufferSize = 60000;         // 10 minutes at 100 Hz
        int trackingDataBufferSize = 18000;    // 10 minutes at 30 Hz
        int weaponStatusBufferSize = 3600;     // 1 hour at 1 Hz
        int cameraStatusBufferSize = 1800;     // 30 minutes at 1 Hz
        int sensorDataBufferSize = 6000;       // 10 minutes at 10 Hz
        int ballisticDataBufferSize = 1800;    // 30 minutes at 1 Hz
        int userInputBufferSize = 6000;        // 10 minutes at 10 Hz

        // Database settings
        bool enableDatabasePersistence = false;
        QString databasePath = "rcws_history.db";
        int databaseWriteIntervalSec = 60;     // Write to DB every minute
    };

    explicit SystemDataLogger(QObject *parent = nullptr);
    explicit SystemDataLogger(const LoggerConfig& config, QObject *parent = nullptr);
    ~SystemDataLogger();

    // ========================================================================
    // Configuration
    // ========================================================================
    void setConfig(const LoggerConfig& config);
    LoggerConfig getConfig() const { return m_config; }

    /**
     * @brief Enable or disable database persistence
     */
    void setDatabasePersistence(bool enabled);

    // ========================================================================
    // Data Query Methods - Retrieve historical data
    // ========================================================================

    /**
     * @brief Get device status data within time range
     */
    QVector<DeviceStatusData> getDeviceStatusHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get gimbal motion data within time range
     */
    QVector<GimbalMotionData> getGimbalMotionHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get IMU data within time range
     */
    QVector<ImuDataPoint> getImuHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get tracking data within time range
     */
    QVector<TrackingDataPoint> getTrackingHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get weapon status data within time range
     */
    QVector<WeaponStatusData> getWeaponStatusHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get camera status data within time range
     */
    QVector<CameraStatusData> getCameraStatusHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get sensor data within time range
     */
    QVector<SensorDataPoint> getSensorHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get ballistic data within time range
     */
    QVector<BallisticDataPoint> getBallisticHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    /**
     * @brief Get user input data within time range
     */
    QVector<UserInputData> getUserInputHistory(
        const QDateTime& startTime,
        const QDateTime& endTime) const;

    // ========================================================================
    // Statistics and Analysis
    // ========================================================================

    /**
     * @brief Get the time range of available data for a category
     */
    QPair<QDateTime, QDateTime> getDataTimeRange(DataCategory category) const;

    /**
     * @brief Get the number of samples stored for a category
     */
    int getSampleCount(DataCategory category) const;

    /**
     * @brief Get memory usage statistics
     */
    struct MemoryStats {
        qint64 totalBytes = 0;
        qint64 deviceStatusBytes = 0;
        qint64 gimbalMotionBytes = 0;
        qint64 imuDataBytes = 0;
        qint64 trackingDataBytes = 0;
        qint64 weaponStatusBytes = 0;
        qint64 cameraStatusBytes = 0;
        qint64 sensorDataBytes = 0;
        qint64 ballisticDataBytes = 0;
        qint64 userInputBytes = 0;
    };
    MemoryStats getMemoryUsage() const;

    // ========================================================================
    // Data Management
    // ========================================================================

    /**
     * @brief Clear all logged data
     */
    void clearAllData();

    /**
     * @brief Clear data for a specific category
     */
    void clearCategory(DataCategory category);

    /**
     * @brief Clear data older than specified time
     */
    void clearDataOlderThan(const QDateTime& cutoffTime);

    /**
     * @brief Export data to CSV file
     */
    bool exportToCSV(DataCategory category, const QString& filePath,
                     const QDateTime& startTime, const QDateTime& endTime) const;

public slots:
    /**
     * @brief Main slot to receive system state updates
     * This automatically extracts relevant data into appropriate categories
     */
    void onSystemStateChanged(const SystemStateData& state);

signals:
    /**
     * @brief Emitted when new data is logged
     */
    void dataLogged(DataCategory category, const QDateTime& timestamp);

    /**
     * @brief Emitted when buffer is full and old data is discarded
     */
    void bufferOverflow(DataCategory category, int droppedSamples);

    /**
     * @brief Emitted on database write completion
     */
    void databaseWriteComplete(int recordsWritten);

private:
    // Ring buffer implementation for each category
    template<typename T>
    class RingBuffer {
    public:
        explicit RingBuffer(int maxSize) : m_maxSize(maxSize) {
            m_data.reserve(maxSize);
        }

        void append(const T& item) {
            QMutexLocker locker(&m_mutex);
            if (m_data.size() >= m_maxSize) {
                m_data.removeFirst();
            }
            m_data.append(item);
        }

        QVector<T> getRange(qint64 startMs, qint64 endMs) const {
            QMutexLocker locker(&m_mutex);
            QVector<T> result;
            for (const auto& item : m_data) {
                if (item.timestampMs >= startMs && item.timestampMs <= endMs) {
                    result.append(item);
                }
            }
            return result;
        }

        QVector<T> getAll() const {
            QMutexLocker locker(&m_mutex);
            return m_data;
        }

        int size() const {
            QMutexLocker locker(&m_mutex);
            return m_data.size();
        }

        void clear() {
            QMutexLocker locker(&m_mutex);
            m_data.clear();
        }

        QPair<qint64, qint64> getTimeRange() const {
            QMutexLocker locker(&m_mutex);
            if (m_data.isEmpty()) return {0, 0};
            return {m_data.first().timestampMs, m_data.last().timestampMs};
        }

    private:
        QVector<T> m_data;
        int m_maxSize;
        mutable QMutex m_mutex;
    };

    // Data buffers for each category
    RingBuffer<DeviceStatusData> m_deviceStatusBuffer;
    RingBuffer<GimbalMotionData> m_gimbalMotionBuffer;
    RingBuffer<ImuDataPoint> m_imuDataBuffer;
    RingBuffer<TrackingDataPoint> m_trackingDataBuffer;
    RingBuffer<WeaponStatusData> m_weaponStatusBuffer;
    RingBuffer<CameraStatusData> m_cameraStatusBuffer;
    RingBuffer<SensorDataPoint> m_sensorDataBuffer;
    RingBuffer<BallisticDataPoint> m_ballisticDataBuffer;
    RingBuffer<UserInputData> m_userInputBuffer;

    // Configuration
    LoggerConfig m_config;

    // Database support
    QSqlDatabase m_database;
    bool m_databaseEnabled;
    QTimer* m_databaseWriteTimer;
    QAtomicInt m_databaseWriteInProgress;

    // Track last written timestamp for each category (for incremental writes)
    qint64 m_lastWrittenTimestamp_deviceStatus;
    qint64 m_lastWrittenTimestamp_gimbalMotion;
    qint64 m_lastWrittenTimestamp_imuData;
    qint64 m_lastWrittenTimestamp_trackingData;
    qint64 m_lastWrittenTimestamp_weaponStatus;
    qint64 m_lastWrittenTimestamp_cameraStatus;
    qint64 m_lastWrittenTimestamp_sensorData;
    qint64 m_lastWrittenTimestamp_ballisticData;
    qint64 m_lastWrittenTimestamp_userInput;

    // Private helper methods
    void initializeBuffers();
    void initializeDatabase();
    void writePendingDataToDatabase();
    void cleanupOldData();  // Delete data older than retention period

    // Data extraction from SystemStateData
    DeviceStatusData extractDeviceStatus(const SystemStateData& state);
    GimbalMotionData extractGimbalMotion(const SystemStateData& state);
    ImuDataPoint extractImuData(const SystemStateData& state);
    TrackingDataPoint extractTrackingData(const SystemStateData& state);
    WeaponStatusData extractWeaponStatus(const SystemStateData& state);
    CameraStatusData extractCameraStatus(const SystemStateData& state);
    SensorDataPoint extractSensorData(const SystemStateData& state);
    BallisticDataPoint extractBallisticData(const SystemStateData& state);
    UserInputData extractUserInput(const SystemStateData& state);

private slots:
    /**
     * @brief Timer slot for periodic database writes (runs on background thread)
     */
    void onDatabaseWriteTimerTimeout();

    /**
     * @brief Called when background database write completes
     */
    void onBackgroundWriteFinished(int recordsWritten);
};

#endif // SYSTEMDATALOGGER_H
