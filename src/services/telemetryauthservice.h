#ifndef TELEMETRYAUTHSERVICE_H
#define TELEMETRYAUTHSERVICE_H

/**
 * @file TelemetryAuthService.h
 * @brief JWT-based authentication and authorization service for telemetry API
 *
 * This service provides secure authentication and role-based access control
 * for the RCWS telemetry system. It implements JWT (JSON Web Token) authentication
 * with configurable roles and permissions.
 *
 * SECURITY FEATURES:
 * • JWT token generation and validation
 * • Role-based access control (Admin, Operator, Viewer)
 * • Token expiration and refresh
 * • IP address whitelisting
 * • Audit logging for all authentication events
 * • Secure password hashing (PBKDF2)
 *
 * ROLES AND PERMISSIONS:
 * • Admin: Full access (read/write, user management, configuration)
 * • Operator: Read/write access to operational data
 * • Viewer: Read-only access to telemetry data
 *
 * @author RCWS Development Team
 * @date 2025
 */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QMutex>

// ============================================================================
// ENUMERATIONS
// ============================================================================

/**
 * @brief User roles with different permission levels
 */
enum class UserRole {
    Viewer,     ///< Read-only access to telemetry data
    Operator,   ///< Read/write access to operational data
    Admin       ///< Full system access including user management
};

/**
 * @brief Permission types for fine-grained access control
 */
enum class Permission {
    ReadTelemetry,      ///< Read telemetry data
    ReadHistory,        ///< Read historical data
    ExportData,         ///< Export data to CSV
    ReadSystemHealth,   ///< Read system health status
    ManageUsers,        ///< Create/delete users
    ModifyConfig        ///< Modify system configuration
};

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief User account information
 */
struct UserAccount {
    QString username;
    QString passwordHash;  ///< PBKDF2 hash
    QString salt;          ///< Random salt for password hashing
    UserRole role;
    bool enabled;
    QDateTime createdAt;
    QDateTime lastLogin;
    QString description;   ///< Optional description/notes

    UserAccount()
        : role(UserRole::Viewer)
        , enabled(true)
        , createdAt(QDateTime::currentDateTime())
    {}
};

/**
 * @brief JWT token payload
 */
struct TokenPayload {
    QString username;
    UserRole role;
    QDateTime issuedAt;
    QDateTime expiresAt;
    QString jti;  ///< JWT ID (unique identifier)

    bool isValid() const {
        return QDateTime::currentDateTime() < expiresAt;
    }

    qint64 remainingSeconds() const {
        return QDateTime::currentDateTime().secsTo(expiresAt);
    }
};

/**
 * @brief Authentication result
 */
struct AuthResult {
    bool success;
    QString token;
    QString errorMessage;
    UserRole role;
    QDateTime expiresAt;

    AuthResult() : success(false), role(UserRole::Viewer) {}
};

/**
 * @brief Audit log entry
 */
struct AuditLogEntry {
    QDateTime timestamp;
    QString username;
    QString action;         ///< e.g., "LOGIN", "LOGOUT", "ACCESS_DENIED"
    QString ipAddress;
    QString endpoint;       ///< API endpoint accessed
    bool success;
    QString details;

    AuditLogEntry()
        : timestamp(QDateTime::currentDateTime())
        , success(false)
    {}
};

// ============================================================================
// MAIN SERVICE CLASS
// ============================================================================

/**
 * @brief Authentication and authorization service for telemetry API
 *
 * This service manages user authentication using JWT tokens and provides
 * role-based access control for the telemetry system.
 */
class TelemetryAuthService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Configuration for authentication service
     */
    struct Config {
        QString jwtSecret;              ///< Secret key for JWT signing (keep secure!)
        int tokenExpirationMinutes;     ///< Token validity period (default: 60 minutes)
        int maxLoginAttempts;           ///< Max failed login attempts before lockout (default: 5)
        int lockoutDurationMinutes;     ///< Lockout duration after max attempts (default: 15)
        bool enableIpWhitelist;         ///< Enable IP address whitelisting
        QStringList allowedIpAddresses; ///< Whitelist of allowed IP addresses/ranges
        bool enableAuditLogging;        ///< Enable audit logging
        QString auditLogPath;           ///< Path to audit log file
        int auditLogMaxSizeMB;          ///< Maximum audit log file size

        Config()
            : jwtSecret("CHANGE_THIS_SECRET_KEY_IN_PRODUCTION")  // Must be changed!
            , tokenExpirationMinutes(60)
            , maxLoginAttempts(5)
            , lockoutDurationMinutes(15)
            , enableIpWhitelist(false)
            , enableAuditLogging(true)
            , auditLogPath("./logs/telemetry_audit.log")
            , auditLogMaxSizeMB(100)
        {}
    };

    explicit TelemetryAuthService(QObject *parent = nullptr);
    explicit TelemetryAuthService(const Config& config, QObject *parent = nullptr);
    ~TelemetryAuthService();

    // ========================================================================
    // Configuration
    // ========================================================================

    void setConfig(const Config& config);
    Config getConfig() const { return m_config; }

    /**
     * @brief Set JWT secret key (MUST be changed from default in production)
     */
    void setJwtSecret(const QString& secret);

    /**
     * @brief Add IP address to whitelist
     */
    void addAllowedIpAddress(const QString& ipAddress);

    /**
     * @brief Remove IP address from whitelist
     */
    void removeAllowedIpAddress(const QString& ipAddress);

    // ========================================================================
    // Authentication
    // ========================================================================

    /**
     * @brief Authenticate user and generate JWT token
     * @param username User's username
     * @param password User's password (plain text - will be hashed)
     * @param clientIp Client's IP address for logging and whitelist check
     * @return AuthResult containing token if successful
     */
    AuthResult authenticate(const QString& username, const QString& password,
                           const QString& clientIp = QString());

    /**
     * @brief Validate JWT token
     * @param token JWT token string
     * @return TokenPayload if valid, empty payload if invalid
     */
    TokenPayload validateToken(const QString& token) const;

    /**
     * @brief Check if token is valid (not expired, correct signature)
     */
    bool isTokenValid(const QString& token) const;

    /**
     * @brief Refresh token (generate new token with extended expiration)
     * @param oldToken Current valid token
     * @return New token string, or empty if old token is invalid
     */
    QString refreshToken(const QString& oldToken);

    /**
     * @brief Revoke token (add to blacklist)
     */
    void revokeToken(const QString& token);

    /**
     * @brief Check if token is blacklisted
     */
    bool isTokenRevoked(const QString& token) const;

    // ========================================================================
    // Authorization
    // ========================================================================

    /**
     * @brief Check if user has required permission
     * @param token JWT token
     * @param permission Required permission
     * @return true if user has permission
     */
    bool hasPermission(const QString& token, Permission permission) const;

    /**
     * @brief Check if IP address is whitelisted
     */
    bool isIpAllowed(const QString& ipAddress) const;

    /**
     * @brief Get user role from token
     */
    UserRole getUserRole(const QString& token) const;

    // ========================================================================
    // User Management
    // ========================================================================

    /**
     * @brief Create new user account
     * @param username Unique username
     * @param password Plain text password (will be hashed)
     * @param role User role
     * @param description Optional description
     * @return true if user created successfully
     */
    bool createUser(const QString& username, const QString& password,
                   UserRole role, const QString& description = QString());

    /**
     * @brief Delete user account
     */
    bool deleteUser(const QString& username);

    /**
     * @brief Change user password
     */
    bool changePassword(const QString& username, const QString& oldPassword,
                       const QString& newPassword);

    /**
     * @brief Enable/disable user account
     */
    bool setUserEnabled(const QString& username, bool enabled);

    /**
     * @brief Get user account information
     */
    UserAccount getUserAccount(const QString& username) const;

    /**
     * @brief Get list of all users
     */
    QList<UserAccount> getAllUsers() const;

    /**
     * @brief Check if user exists
     */
    bool userExists(const QString& username) const;

    // ========================================================================
    // Audit Logging
    // ========================================================================

    /**
     * @brief Log authentication/authorization event
     */
    void logAuditEvent(const QString& username, const QString& action,
                      const QString& ipAddress, const QString& endpoint,
                      bool success, const QString& details = QString());

    /**
     * @brief Get recent audit log entries
     */
    QList<AuditLogEntry> getRecentAuditLogs(int maxEntries = 100) const;

    /**
     * @brief Clear audit log
     */
    void clearAuditLog();

    // ========================================================================
    // Persistence
    // ========================================================================

    /**
     * @brief Load users from file (encrypted JSON)
     */
    bool loadUsers(const QString& filePath);

    /**
     * @brief Save users to file (encrypted JSON)
     */
    bool saveUsers(const QString& filePath) const;

signals:
    /**
     * @brief Emitted when user successfully authenticates
     */
    void userAuthenticated(const QString& username, UserRole role, const QString& ipAddress);

    /**
     * @brief Emitted when authentication fails
     */
    void authenticationFailed(const QString& username, const QString& ipAddress, const QString& reason);

    /**
     * @brief Emitted when user is locked out due to failed attempts
     */
    void userLockedOut(const QString& username, const QString& ipAddress);

    /**
     * @brief Emitted when audit event is logged
     */
    void auditEventLogged(const AuditLogEntry& entry);

private:
    // Configuration
    Config m_config;

    // User database (in-memory, persisted to file)
    QMap<QString, UserAccount> m_users;

    // Token blacklist (revoked tokens)
    QSet<QString> m_revokedTokens;

    // Failed login tracking
    struct LoginAttempt {
        int failedAttempts;
        QDateTime lockoutUntil;
    };
    QMap<QString, LoginAttempt> m_loginAttempts;

    // Audit log (in-memory buffer)
    QList<AuditLogEntry> m_auditLog;
    mutable QMutex m_auditLogMutex;

    // Helper methods
    QString generateToken(const QString& username, UserRole role) const;
    TokenPayload decodeToken(const QString& token) const;
    QString hashPassword(const QString& password, const QString& salt) const;
    QString generateSalt() const;
    QString generateJti() const;
    bool verifyPassword(const QString& password, const QString& hash, const QString& salt) const;
    void writeAuditLogToFile(const AuditLogEntry& entry);
    void checkAndRotateAuditLog();

    // JWT Helper methods
    QString base64UrlEncode(const QByteArray& data) const;
    QByteArray base64UrlDecode(const QString& data) const;
    QString createHmacSignature(const QString& data) const;
    bool verifyHmacSignature(const QString& data, const QString& signature) const;

    // Permission mapping
    QSet<Permission> getPermissionsForRole(UserRole role) const;
};

#endif // TELEMETRYAUTHSERVICE_H
