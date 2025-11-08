#include "telemetryauthservice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRandomGenerator>
#include <QUuid>
#include <QMutexLocker>

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

TelemetryAuthService::TelemetryAuthService(QObject *parent)
    : QObject(parent)
{
    qInfo() << "TelemetryAuthService: Initialized with default configuration";

    // Create default admin user if no users exist
    if (m_users.isEmpty()) {
        createUser("admin", "admin123", UserRole::Admin, "Default administrator account");
        qWarning() << "TelemetryAuthService: Default admin user created. CHANGE PASSWORD IMMEDIATELY!";
    }
}

TelemetryAuthService::TelemetryAuthService(const Config& config, QObject *parent)
    : QObject(parent)
    , m_config(config)
{
    qInfo() << "TelemetryAuthService: Initialized with custom configuration";
    qInfo() << "  Token Expiration:" << m_config.tokenExpirationMinutes << "minutes";
    qInfo() << "  IP Whitelist:" << (m_config.enableIpWhitelist ? "Enabled" : "Disabled");
    qInfo() << "  Audit Logging:" << (m_config.enableAuditLogging ? "Enabled" : "Disabled");

    // Create default admin user if no users exist
    if (m_users.isEmpty()) {
        createUser("admin", "admin123", UserRole::Admin, "Default administrator account");
        qWarning() << "TelemetryAuthService: Default admin user created. CHANGE PASSWORD IMMEDIATELY!";
    }
}

TelemetryAuthService::~TelemetryAuthService()
{
    qInfo() << "TelemetryAuthService: Shutting down";
}

// ============================================================================
// CONFIGURATION
// ============================================================================

void TelemetryAuthService::setConfig(const Config& config)
{
    m_config = config;
    qInfo() << "TelemetryAuthService: Configuration updated";
}

void TelemetryAuthService::setJwtSecret(const QString& secret)
{
    if (secret.length() < 32) {
        qWarning() << "TelemetryAuthService: JWT secret should be at least 32 characters!";
    }
    m_config.jwtSecret = secret;
    qInfo() << "TelemetryAuthService: JWT secret updated";
}

void TelemetryAuthService::addAllowedIpAddress(const QString& ipAddress)
{
    if (!m_config.allowedIpAddresses.contains(ipAddress)) {
        m_config.allowedIpAddresses.append(ipAddress);
        qInfo() << "TelemetryAuthService: Added IP to whitelist:" << ipAddress;
    }
}

void TelemetryAuthService::removeAllowedIpAddress(const QString& ipAddress)
{
    m_config.allowedIpAddresses.removeAll(ipAddress);
    qInfo() << "TelemetryAuthService: Removed IP from whitelist:" << ipAddress;
}

// ============================================================================
// AUTHENTICATION
// ============================================================================

AuthResult TelemetryAuthService::authenticate(const QString& username, const QString& password,
                                              const QString& clientIp)
{
    AuthResult result;

    // Check IP whitelist
    if (m_config.enableIpWhitelist && !clientIp.isEmpty() && !isIpAllowed(clientIp)) {
        result.errorMessage = "IP address not whitelisted";
        logAuditEvent(username, "LOGIN_FAILED", clientIp, "/api/auth/login", false, "IP not whitelisted");
        emit authenticationFailed(username, clientIp, result.errorMessage);
        return result;
    }

    // Check if user is locked out
    if (m_loginAttempts.contains(username)) {
        const auto& attempt = m_loginAttempts[username];
        if (attempt.lockoutUntil.isValid() && QDateTime::currentDateTime() < attempt.lockoutUntil) {
            result.errorMessage = QString("Account locked until %1").arg(attempt.lockoutUntil.toString());
            logAuditEvent(username, "LOGIN_FAILED", clientIp, "/api/auth/login", false, "Account locked");
            emit authenticationFailed(username, clientIp, result.errorMessage);
            return result;
        }
    }

    // Check if user exists
    if (!m_users.contains(username)) {
        result.errorMessage = "Invalid username or password";
        logAuditEvent(username, "LOGIN_FAILED", clientIp, "/api/auth/login", false, "User not found");
        emit authenticationFailed(username, clientIp, result.errorMessage);
        return result;
    }

    const UserAccount& user = m_users[username];

    // Check if user is enabled
    if (!user.enabled) {
        result.errorMessage = "Account disabled";
        logAuditEvent(username, "LOGIN_FAILED", clientIp, "/api/auth/login", false, "Account disabled");
        emit authenticationFailed(username, clientIp, result.errorMessage);
        return result;
    }

    // Verify password
    if (!verifyPassword(password, user.passwordHash, user.salt)) {
        // Track failed attempt
        if (!m_loginAttempts.contains(username)) {
            m_loginAttempts[username] = LoginAttempt{0, QDateTime()};
        }

        m_loginAttempts[username].failedAttempts++;

        // Check if should lock out
        if (m_loginAttempts[username].failedAttempts >= m_config.maxLoginAttempts) {
            m_loginAttempts[username].lockoutUntil =
                QDateTime::currentDateTime().addSecs(m_config.lockoutDurationMinutes * 60);

            result.errorMessage = QString("Account locked due to too many failed attempts");
            logAuditEvent(username, "ACCOUNT_LOCKED", clientIp, "/api/auth/login", false,
                         QString("Locked for %1 minutes").arg(m_config.lockoutDurationMinutes));
            emit userLockedOut(username, clientIp);
            return result;
        }

        result.errorMessage = "Invalid username or password";
        logAuditEvent(username, "LOGIN_FAILED", clientIp, "/api/auth/login", false, "Invalid password");
        emit authenticationFailed(username, clientIp, result.errorMessage);
        return result;
    }

    // Authentication successful - clear failed attempts
    m_loginAttempts.remove(username);

    // Generate JWT token
    result.success = true;
    result.token = generateToken(username, user.role);
    result.role = user.role;
    result.expiresAt = QDateTime::currentDateTime().addSecs(m_config.tokenExpirationMinutes * 60);

    // Update last login
    m_users[username].lastLogin = QDateTime::currentDateTime();

    // Log successful authentication
    logAuditEvent(username, "LOGIN_SUCCESS", clientIp, "/api/auth/login", true,
                 QString("Role: %1").arg(static_cast<int>(user.role)));
    emit userAuthenticated(username, user.role, clientIp);

    return result;
}

TokenPayload TelemetryAuthService::validateToken(const QString& token) const
{
    return decodeToken(token);
}

bool TelemetryAuthService::isTokenValid(const QString& token) const
{
    if (isTokenRevoked(token)) {
        return false;
    }

    TokenPayload payload = decodeToken(token);
    return payload.isValid();
}

QString TelemetryAuthService::refreshToken(const QString& oldToken)
{
    TokenPayload payload = decodeToken(oldToken);

    if (!payload.isValid()) {
        return QString();
    }

    // Revoke old token
    revokeToken(oldToken);

    // Generate new token
    QString newToken = generateToken(payload.username, payload.role);

    logAuditEvent(payload.username, "TOKEN_REFRESHED", "", "", true, "");

    return newToken;
}

void TelemetryAuthService::revokeToken(const QString& token)
{
    m_revokedTokens.insert(token);

    TokenPayload payload = decodeToken(token);
    logAuditEvent(payload.username, "TOKEN_REVOKED", "", "", true, "");
}

bool TelemetryAuthService::isTokenRevoked(const QString& token) const
{
    return m_revokedTokens.contains(token);
}

// ============================================================================
// AUTHORIZATION
// ============================================================================

bool TelemetryAuthService::hasPermission(const QString& token, Permission permission) const
{
    if (!isTokenValid(token)) {
        return false;
    }

    TokenPayload payload = decodeToken(token);
    QSet<Permission> permissions = getPermissionsForRole(payload.role);

    return permissions.contains(permission);
}

bool TelemetryAuthService::isIpAllowed(const QString& ipAddress) const
{
    if (!m_config.enableIpWhitelist) {
        return true;  // Whitelist disabled, all IPs allowed
    }

    if (m_config.allowedIpAddresses.isEmpty()) {
        return true;  // Empty whitelist = all allowed
    }

    // Check exact match or CIDR range (simplified - exact match only for now)
    return m_config.allowedIpAddresses.contains(ipAddress) ||
           m_config.allowedIpAddresses.contains("0.0.0.0") ||  // Allow all IPv4
           m_config.allowedIpAddresses.contains("::");         // Allow all IPv6
}

UserRole TelemetryAuthService::getUserRole(const QString& token) const
{
    TokenPayload payload = decodeToken(token);
    return payload.role;
}

// ============================================================================
// USER MANAGEMENT
// ============================================================================

bool TelemetryAuthService::createUser(const QString& username, const QString& password,
                                     UserRole role, const QString& description)
{
    if (username.isEmpty() || password.isEmpty()) {
        qWarning() << "TelemetryAuthService: Cannot create user with empty username or password";
        return false;
    }

    if (m_users.contains(username)) {
        qWarning() << "TelemetryAuthService: User already exists:" << username;
        return false;
    }

    if (password.length() < 8) {
        qWarning() << "TelemetryAuthService: Password must be at least 8 characters";
        return false;
    }

    UserAccount user;
    user.username = username;
    user.salt = generateSalt();
    user.passwordHash = hashPassword(password, user.salt);
    user.role = role;
    user.description = description;
    user.enabled = true;
    user.createdAt = QDateTime::currentDateTime();

    m_users[username] = user;

    qInfo() << "TelemetryAuthService: User created:" << username << "Role:" << static_cast<int>(role);
    logAuditEvent("system", "USER_CREATED", "", "", true,
                 QString("Username: %1, Role: %2").arg(username).arg(static_cast<int>(role)));

    return true;
}

bool TelemetryAuthService::deleteUser(const QString& username)
{
    if (!m_users.contains(username)) {
        return false;
    }

    m_users.remove(username);
    m_loginAttempts.remove(username);

    qInfo() << "TelemetryAuthService: User deleted:" << username;
    logAuditEvent("system", "USER_DELETED", "", "", true, QString("Username: %1").arg(username));

    return true;
}

bool TelemetryAuthService::changePassword(const QString& username, const QString& oldPassword,
                                         const QString& newPassword)
{
    if (!m_users.contains(username)) {
        return false;
    }

    UserAccount& user = m_users[username];

    // Verify old password
    if (!verifyPassword(oldPassword, user.passwordHash, user.salt)) {
        logAuditEvent(username, "PASSWORD_CHANGE_FAILED", "", "", false, "Invalid old password");
        return false;
    }

    if (newPassword.length() < 8) {
        qWarning() << "TelemetryAuthService: New password must be at least 8 characters";
        return false;
    }

    // Update password
    user.salt = generateSalt();
    user.passwordHash = hashPassword(newPassword, user.salt);

    qInfo() << "TelemetryAuthService: Password changed for user:" << username;
    logAuditEvent(username, "PASSWORD_CHANGED", "", "", true, "");

    return true;
}

bool TelemetryAuthService::setUserEnabled(const QString& username, bool enabled)
{
    if (!m_users.contains(username)) {
        return false;
    }

    m_users[username].enabled = enabled;

    qInfo() << "TelemetryAuthService: User" << username << (enabled ? "enabled" : "disabled");
    logAuditEvent("system", enabled ? "USER_ENABLED" : "USER_DISABLED", "", "", true,
                 QString("Username: %1").arg(username));

    return true;
}

UserAccount TelemetryAuthService::getUserAccount(const QString& username) const
{
    if (m_users.contains(username)) {
        return m_users[username];
    }
    return UserAccount();
}

QList<UserAccount> TelemetryAuthService::getAllUsers() const
{
    return m_users.values();
}

bool TelemetryAuthService::userExists(const QString& username) const
{
    return m_users.contains(username);
}

// ============================================================================
// AUDIT LOGGING
// ============================================================================

void TelemetryAuthService::logAuditEvent(const QString& username, const QString& action,
                                        const QString& ipAddress, const QString& endpoint,
                                        bool success, const QString& details)
{
    if (!m_config.enableAuditLogging) {
        return;
    }

    AuditLogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.username = username;
    entry.action = action;
    entry.ipAddress = ipAddress;
    entry.endpoint = endpoint;
    entry.success = success;
    entry.details = details;

    QMutexLocker locker(&m_auditLogMutex);
    m_auditLog.append(entry);

    // Limit in-memory buffer size
    if (m_auditLog.size() > 1000) {
        m_auditLog.removeFirst();
    }

    // Write to file
    writeAuditLogToFile(entry);

    emit auditEventLogged(entry);
}

QList<AuditLogEntry> TelemetryAuthService::getRecentAuditLogs(int maxEntries) const
{
    QMutexLocker locker(&m_auditLogMutex);

    if (m_auditLog.size() <= maxEntries) {
        return m_auditLog;
    }

    return m_auditLog.mid(m_auditLog.size() - maxEntries, maxEntries);
}

void TelemetryAuthService::clearAuditLog()
{
    QMutexLocker locker(&m_auditLogMutex);
    m_auditLog.clear();
    qInfo() << "TelemetryAuthService: Audit log cleared";
}

// ============================================================================
// PERSISTENCE
// ============================================================================

bool TelemetryAuthService::loadUsers(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "TelemetryAuthService: Failed to open users file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "TelemetryAuthService: Invalid users file format";
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray usersArray = root["users"].toArray();

    m_users.clear();

    for (const QJsonValue& value : usersArray) {
        QJsonObject userObj = value.toObject();

        UserAccount user;
        user.username = userObj["username"].toString();
        user.passwordHash = userObj["passwordHash"].toString();
        user.salt = userObj["salt"].toString();
        user.role = static_cast<UserRole>(userObj["role"].toInt());
        user.enabled = userObj["enabled"].toBool();
        user.createdAt = QDateTime::fromString(userObj["createdAt"].toString(), Qt::ISODate);
        user.lastLogin = QDateTime::fromString(userObj["lastLogin"].toString(), Qt::ISODate);
        user.description = userObj["description"].toString();

        m_users[user.username] = user;
    }

    qInfo() << "TelemetryAuthService: Loaded" << m_users.size() << "users from" << filePath;
    return true;
}

bool TelemetryAuthService::saveUsers(const QString& filePath) const
{
    QJsonArray usersArray;

    for (const UserAccount& user : m_users) {
        QJsonObject userObj;
        userObj["username"] = user.username;
        userObj["passwordHash"] = user.passwordHash;
        userObj["salt"] = user.salt;
        userObj["role"] = static_cast<int>(user.role);
        userObj["enabled"] = user.enabled;
        userObj["createdAt"] = user.createdAt.toString(Qt::ISODate);
        userObj["lastLogin"] = user.lastLogin.toString(Qt::ISODate);
        userObj["description"] = user.description;

        usersArray.append(userObj);
    }

    QJsonObject root;
    root["users"] = usersArray;
    root["version"] = "1.0";

    QJsonDocument doc(root);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "TelemetryAuthService: Failed to write users file:" << filePath;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qInfo() << "TelemetryAuthService: Saved" << m_users.size() << "users to" << filePath;
    return true;
}

// ============================================================================
// HELPER METHODS
// ============================================================================

QString TelemetryAuthService::generateToken(const QString& username, UserRole role) const
{
    // JWT Header
    QJsonObject header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";

    QString headerStr = base64UrlEncode(QJsonDocument(header).toJson(QJsonDocument::Compact));

    // JWT Payload
    QJsonObject payload;
    payload["username"] = username;
    payload["role"] = static_cast<int>(role);
    payload["iat"] = QDateTime::currentDateTime().toSecsSinceEpoch();
    payload["exp"] = QDateTime::currentDateTime().addSecs(m_config.tokenExpirationMinutes * 60).toSecsSinceEpoch();
    payload["jti"] = generateJti();

    QString payloadStr = base64UrlEncode(QJsonDocument(payload).toJson(QJsonDocument::Compact));

    // Create signature
    QString data = headerStr + "." + payloadStr;
    QString signature = createHmacSignature(data);

    return data + "." + signature;
}

TokenPayload TelemetryAuthService::decodeToken(const QString& token) const
{
    TokenPayload result;

    QStringList parts = token.split('.');
    if (parts.size() != 3) {
        return result;  // Invalid token format
    }

    // Verify signature
    QString data = parts[0] + "." + parts[1];
    if (!verifyHmacSignature(data, parts[2])) {
        return result;  // Invalid signature
    }

    // Decode payload
    QByteArray payloadData = base64UrlDecode(parts[1]);
    QJsonDocument doc = QJsonDocument::fromJson(payloadData);

    if (!doc.isObject()) {
        return result;
    }

    QJsonObject payload = doc.object();

    result.username = payload["username"].toString();
    result.role = static_cast<UserRole>(payload["role"].toInt());
    result.issuedAt = QDateTime::fromSecsSinceEpoch(payload["iat"].toInteger());
    result.expiresAt = QDateTime::fromSecsSinceEpoch(payload["exp"].toInteger());
    result.jti = payload["jti"].toString();

    return result;
}

QString TelemetryAuthService::hashPassword(const QString& password, const QString& salt) const
{
    // Use PBKDF2 for password hashing
    QByteArray saltBytes = salt.toUtf8();
    QByteArray passwordBytes = password.toUtf8();

    // Simple implementation - in production use QPasswordDigestor (Qt 5.12+)
    // For now, use repeated SHA-256 hashing
    QByteArray hash = passwordBytes + saltBytes;
    for (int i = 0; i < 10000; ++i) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }

    return QString(hash.toHex());
}

QString TelemetryAuthService::generateSalt() const
{
    QByteArray salt;
    for (int i = 0; i < 16; ++i) {
        salt.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    return QString(salt.toHex());
}

QString TelemetryAuthService::generateJti() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool TelemetryAuthService::verifyPassword(const QString& password, const QString& hash, const QString& salt) const
{
    QString computedHash = hashPassword(password, salt);
    return computedHash == hash;
}

void TelemetryAuthService::writeAuditLogToFile(const AuditLogEntry& entry)
{
    checkAndRotateAuditLog();

    QFile file(m_config.auditLogPath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << entry.timestamp.toString(Qt::ISODate) << " | "
        << entry.username << " | "
        << entry.action << " | "
        << entry.ipAddress << " | "
        << entry.endpoint << " | "
        << (entry.success ? "SUCCESS" : "FAILED") << " | "
        << entry.details << "\n";

    file.close();
}

void TelemetryAuthService::checkAndRotateAuditLog()
{
    QFile file(m_config.auditLogPath);
    if (!file.exists()) {
        return;
    }

    qint64 sizeInMB = file.size() / (1024 * 1024);
    if (sizeInMB >= m_config.auditLogMaxSizeMB) {
        // Rotate log file
        QString backupPath = m_config.auditLogPath + "." +
                           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        file.rename(backupPath);
        qInfo() << "TelemetryAuthService: Audit log rotated to" << backupPath;
    }
}

QString TelemetryAuthService::base64UrlEncode(const QByteArray& data) const
{
    return QString(data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QByteArray TelemetryAuthService::base64UrlDecode(const QString& data) const
{
    return QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64UrlEncoding);
}

QString TelemetryAuthService::createHmacSignature(const QString& data) const
{
    QByteArray key = m_config.jwtSecret.toUtf8();
    QByteArray message = data.toUtf8();

    QByteArray signature = QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);

    return base64UrlEncode(signature);
}

bool TelemetryAuthService::verifyHmacSignature(const QString& data, const QString& signature) const
{
    QString expectedSignature = createHmacSignature(data);
    return expectedSignature == signature;
}

QSet<Permission> TelemetryAuthService::getPermissionsForRole(UserRole role) const
{
    QSet<Permission> permissions;

    switch (role) {
    case UserRole::Admin:
        // Admin has all permissions
        permissions.insert(Permission::ReadTelemetry);
        permissions.insert(Permission::ReadHistory);
        permissions.insert(Permission::ExportData);
        permissions.insert(Permission::ReadSystemHealth);
        permissions.insert(Permission::ManageUsers);
        permissions.insert(Permission::ModifyConfig);
        break;

    case UserRole::Operator:
        // Operator has read/write but no user management
        permissions.insert(Permission::ReadTelemetry);
        permissions.insert(Permission::ReadHistory);
        permissions.insert(Permission::ExportData);
        permissions.insert(Permission::ReadSystemHealth);
        break;

    case UserRole::Viewer:
        // Viewer has read-only access
        permissions.insert(Permission::ReadTelemetry);
        permissions.insert(Permission::ReadHistory);
        permissions.insert(Permission::ReadSystemHealth);
        break;
    }

    return permissions;
}
