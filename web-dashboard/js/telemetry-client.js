/**
 * RCWS Telemetry Client
 * Handles WebSocket connections and REST API calls with JWT authentication
 */

class TelemetryClient {
    constructor() {
        this.serverUrl = 'http://localhost:8443';
        this.wsUrl = '';
        this.token = null;
        this.username = null;
        this.userRole = null;
        this.ws = null;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 5;
        this.reconnectDelay = 2000;
        this.messageCount = 0;
        this.lastMessageTime = null;
        this.updateRate = 0;

        // Callbacks
        this.onConnected = null;
        this.onDisconnected = null;
        this.onTelemetryUpdate = null;
        this.onError = null;
    }

    /**
     * Authenticate with the server using username/password
     */
    async login(serverUrl, username, password) {
        this.serverUrl = serverUrl.replace(/\/$/, ''); // Remove trailing slash
        this.username = username;

        try {
            const response = await fetch(`${this.serverUrl}/api/auth/login`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ username, password })
            });

            if (response.status !== 200) {
                const error = await response.json();
                throw new Error(error.error || 'Login failed');
            }

            const data = await response.json();
            this.token = data.token;
            this.userRole = data.role;

            console.log('✓ Authentication successful');
            console.log(`  User: ${username}`);
            console.log(`  Role: ${this.userRole}`);
            console.log(`  Token: ${this.token.substring(0, 20)}...`);

            return { success: true, role: this.userRole };
        } catch (error) {
            console.error('Authentication failed:', error);
            throw error;
        }
    }

    /**
     * Connect to WebSocket telemetry stream
     */
    connectWebSocket() {
        if (!this.token) {
            console.error('No authentication token available');
            return;
        }

        // Convert HTTP URL to WebSocket URL
        this.wsUrl = this.serverUrl.replace('http://', 'ws://').replace('https://', 'wss://');
        // Use port 8444 for WebSocket
        this.wsUrl = this.wsUrl.replace(':8443', ':8444') + '/telemetry';

        console.log(`Connecting to WebSocket: ${this.wsUrl}`);

        this.ws = new WebSocket(this.wsUrl);

        this.ws.onopen = () => {
            console.log('✓ WebSocket connected');
            this.reconnectAttempts = 0;

            // Authenticate with JWT token
            this.sendMessage({
                type: 'auth',
                token: this.token
            });

            // Subscribe to all telemetry categories
            setTimeout(() => {
                this.subscribe(['all']);
            }, 500);

            if (this.onConnected) {
                this.onConnected();
            }
        };

        this.ws.onmessage = (event) => {
            try {
                const message = JSON.parse(event.data);
                this.handleMessage(message);
            } catch (error) {
                console.error('Failed to parse WebSocket message:', error);
            }
        };

        this.ws.onerror = (error) => {
            console.error('WebSocket error:', error);
            if (this.onError) {
                this.onError(error);
            }
        };

        this.ws.onclose = () => {
            console.log('✗ WebSocket disconnected');
            if (this.onDisconnected) {
                this.onDisconnected();
            }

            // Attempt reconnection
            if (this.reconnectAttempts < this.maxReconnectAttempts) {
                this.reconnectAttempts++;
                console.log(`Reconnecting in ${this.reconnectDelay/1000}s (attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts})...`);
                setTimeout(() => this.connectWebSocket(), this.reconnectDelay);
            } else {
                console.error('Max reconnection attempts reached');
            }
        };
    }

    /**
     * Subscribe to telemetry categories
     */
    subscribe(categories) {
        this.sendMessage({
            type: 'subscribe',
            categories: categories
        });
        console.log('✓ Subscribed to categories:', categories);
    }

    /**
     * Send ping to keep connection alive
     */
    sendPing() {
        this.sendMessage({
            type: 'ping'
        });
    }

    /**
     * Send message through WebSocket
     */
    sendMessage(message) {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(message));
        } else {
            console.warn('WebSocket not connected, cannot send message');
        }
    }


/**
 * Handle incoming WebSocket message
 */
handleMessage(message) {
    switch (message.type) {
        case 'auth_response':
            if (message.success) {
                console.log('✓ WebSocket authenticated');
            } else {
                console.error('✗ WebSocket authentication failed:', message.error);
            }
            break;

        case 'telemetry':
            this.messageCount++;
            const now = Date.now();
            if (this.lastMessageTime) {
                const delta = (now - this.lastMessageTime) / 1000;
                this.updateRate = (1 / delta).toFixed(1);
            }
            this.lastMessageTime = now;

            // Transform flat data structure to nested structure expected by dashboard
            const transformedData = this.transformTelemetryData(message.data);

            if (this.onTelemetryUpdate) {
                this.onTelemetryUpdate(transformedData);
            }
            break;

        case 'pong':
            // Heartbeat response
            break;

        case 'error':
            console.error('Server error:', message.error);
            if (this.onError) {
                this.onError(message.error);
            }
            break;

        default:
            console.warn('Unknown message type:', message.type);
    }
}

 /**
 * Transform telemetry data to dashboard structure
 * Handles both WebSocket (nested) and REST API (flat) formats
 */
transformTelemetryData(data) {
    //console.log('📡 Transforming telemetry data:', data);
    
    // Check if data is already nested (WebSocket format) or flat (REST API format)
    const isNested = data.gimbal && typeof data.gimbal === 'object';
    
    if (isNested) {
        // WebSocket format - already nested, just rename properties
        return {
            timestamp: data.timestamp,
            gimbal: {
                gimbalAz: data.gimbal.azimuth || 0,
                gimbalEl: data.gimbal.elevation || 0,
                azSpeed: data.gimbal.azimuthSpeed || 0,
                elSpeed: data.gimbal.elevationSpeed || 0,
                gimbalSpeed: data.gimbal.gimbalSpeed || 0,
                opMode: data.gimbal.opMode || 0,
                motionMode: data.gimbal.motionMode || 0
            },
            imu: {
                imuRollDeg: data.imu.roll || 0,
                imuPitchDeg: data.imu.pitch || 0,
                imuYawDeg: data.imu.yaw || 0,
                gyroX: data.imu.gyroX || 0,
                gyroY: data.imu.gyroY || 0,
                gyroZ: data.imu.gyroZ || 0,
                temperature: data.imu.temperature || 0
            },
            device: {
                azMotorTemp: data.device.azMotorTemp || 0,
                azDriverTemp: data.device.azDriverTemp || 0,
                elMotorTemp: data.device.elMotorTemp || 0,
                elDriverTemp: data.device.elDriverTemp || 0,
                panelTemperature: data.device.panelTemp || 0,
                stationTemperature: data.device.stationTemp || 0,
                dayCameraConnected: data.device.dayCameraConnected || false,
                nightCameraConnected: data.device.nightCameraConnected || false,
                emergencyStopActive: data.device.emergencyStop || false
            },
            weapon: {
                gunArmed: data.weapon.armed || false,
                ammoLoaded: data.weapon.ammoLoaded || false,
                fireMode: data.weapon.fireMode || 0,
                ammunitionLevel: data.weapon.ammunitionLevel || 0,
                isReticleInNoFireZone: data.weapon.inNoFireZone || false
            },
            tracking: {
                trackingActive: data.tracking.active || false,
                trackingPhase: data.tracking.phase || 0,
                hasValidTarget: data.tracking.hasTarget || false,
                targetAz: data.tracking.targetAz || 0,
                targetEl: data.tracking.targetEl || 0,
                trackedTargetCenterX_px: data.tracking.targetCenterX || 0,
                trackedTargetCenterY_px: data.tracking.targetCenterY || 0
            },
            camera: {
                activeCameraIsDay: data.camera.activeCamera === 'day',
                dayZoomPosition: data.camera.dayZoom || 0,
                nightZoomPosition: data.camera.nightZoom || 0,
                dayCurrentHFOV: data.camera.dayHFOV || 0,
                nightCurrentHFOV: data.camera.nightHFOV || 0
            },
            sensor: {
                lrfDistance: data.sensor.lrfDistance || 0,
                radarPlotCount: data.sensor.radarPlotCount || 0,
                selectedRadarTrackId: data.sensor.selectedTrackId || 0
            },
            ballistic: {
                zeroingActive: data.ballistic?.zeroingActive || false,
                zeroingAzOffset: data.ballistic?.zeroingAzOffset || 0,
                zeroingElOffset: data.ballistic?.zeroingElOffset || 0,
                windageActive: data.ballistic?.windageActive || false,
                windSpeed: data.ballistic?.windSpeed || 0
            }
        };
    } else {
        // REST API format - flat structure, need to nest it
        return {
            timestamp: data.timestamp,
            gimbal: {
                gimbalAz: data.gimbalAz || 0,
                gimbalEl: data.gimbalEl || 0,
                azSpeed: data.azimuthSpeed || 0,
                elSpeed: data.elevationSpeed || 0,
                gimbalSpeed: data.gimbalSpeed || 0,
                opMode: data.opMode || 0,
                motionMode: data.motionMode || 0
            },
            imu: {
                imuRollDeg: data.roll || 0,
                imuPitchDeg: data.pitch || 0,
                imuYawDeg: data.yaw || 0,
                gyroX: data.gyroX || 0,
                gyroY: data.gyroY || 0,
                gyroZ: data.gyroZ || 0,
                temperature: data.temperature || 0
            },
            device: {
                azMotorTemp: data.azMotorTemp || 0,
                azDriverTemp: data.azDriverTemp || 0,
                elMotorTemp: data.elMotorTemp || 0,
                elDriverTemp: data.elDriverTemp || 0,
                panelTemperature: data.panelTemp || 0,
                stationTemperature: data.stationTemp || 0,
                dayCameraConnected: data.dayCameraConnected || false,
                nightCameraConnected: data.nightCameraConnected || false,
                emergencyStopActive: data.emergencyStop || false
            },
            weapon: {
                gunArmed: data.armed || false,
                ammoLoaded: data.ammoLoaded || false,
                fireMode: data.fireMode || 0,
                ammunitionLevel: data.ammunitionLevel || 0,
                isReticleInNoFireZone: data.inNoFireZone || false
            },
            tracking: {
                trackingActive: data.trackingActive || false,
                trackingPhase: data.trackingPhase || 0,
                hasValidTarget: data.hasValidTarget || false,
                targetAz: data.targetAz || 0,
                targetEl: data.targetEl || 0,
                trackedTargetCenterX_px: data.targetCenterX || 0,
                trackedTargetCenterY_px: data.targetCenterY || 0
            },
            camera: {
                activeCameraIsDay: data.activeCamera === 'day',
                dayZoomPosition: data.dayZoom || 0,
                nightZoomPosition: data.nightZoom || 0,
                dayCurrentHFOV: data.dayHFOV || 0,
                nightCurrentHFOV: data.nightHFOV || 0
            },
            sensor: {
                lrfDistance: data.lrfDistance || 0,
                radarPlotCount: data.radarPlotCount || 0,
                selectedRadarTrackId: data.selectedTrackId || 0
            }
        };
    }
}
    /**
     * Disconnect from WebSocket
     */
    disconnect() {
        if (this.ws) {
            this.ws.close();
            this.ws = null;
        }
    }

    /**
     * Make authenticated REST API call
     */
    async apiCall(endpoint, options = {}) {
        if (!this.token) {
            throw new Error('Not authenticated');
        }

        const url = `${this.serverUrl}${endpoint}`;
        const headers = {
            'Authorization': `Bearer ${this.token}`,
            'Content-Type': 'application/json',
            ...options.headers
        };

        try {
            const response = await fetch(url, {
                ...options,
                headers
            });

            if (response.status === 401) {
                throw new Error('Authentication expired');
            }

            if (response.status !== 200) {
                const error = await response.json();
                throw new Error(error.error || 'API call failed');
            }

            return await response.json();
        } catch (error) {
            console.error(`API call failed (${endpoint}):`, error);
            throw error;
        }
    }

    /**
     * Get current telemetry snapshot
     */
    async getCurrentTelemetry() {
        return await this.apiCall('/api/telemetry/current');
    }

    /**
     * Get gimbal motion history
     * FIXED: Changed endpoint from /gimbal/history to /history/gimbal
     * FIXED: Changed query params from start/end to from/to
     */
    async getGimbalHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/gimbal?${params}`);
    }

    /**
     * Get IMU data history
     * FIXED: Changed endpoint from /imu/history to /history/imu
     * FIXED: Changed query params from start/end to from/to
     */
    async getImuHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/imu?${params}`);
    }

    /**
     * Get device status history
     * FIXED: Changed endpoint from /device/history to /history/device
     * FIXED: Changed query params from start/end to from/to
     */
    async getDeviceHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/device?${params}`);
    }

    /**
     * Get tracking data history
     * FIXED: Changed endpoint from /tracking/history to /history/tracking
     * FIXED: Changed query params from start/end to from/to
     */
    async getTrackingHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/tracking?${params}`);
    }

    /**
     * Get weapon status history
     * FIXED: Changed endpoint from /weapon/history to /history/weapon
     * FIXED: Changed query params from start/end to from/to
     */
    async getWeaponHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/weapon?${params}`);
    }

    /**
     * Get camera status history
     * ADDED: This was missing!
     */
    async getCameraHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/camera?${params}`);
    }

    /**
     * Get sensor data history
     * ADDED: This was missing!
     */
    async getSensorHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/sensor?${params}`);
    }

    /**
     * Get ballistic data history
     * ADDED: This was missing!
     */
    async getBallisticHistory(startTime, endTime) {
        const params = new URLSearchParams({
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/history/ballistic?${params}`);
    }

    /**
     * Export telemetry data to CSV
     * FIXED: Changed endpoint to match C++ server
     * FIXED: Changed query params from start/end to from/to
     */
    async exportToCsv(category, startTime, endTime) {
        const params = new URLSearchParams({
            category,
            from: startTime.toISOString(),
            to: endTime.toISOString()
        });

        const url = `${this.serverUrl}/api/telemetry/export/csv?${params}`;
        const response = await fetch(url, {
            headers: {
                'Authorization': `Bearer ${this.token}`
            }
        });

        if (response.status !== 200) {
            throw new Error('CSV export failed');
        }

        const blob = await response.blob();
        const downloadUrl = window.URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = downloadUrl;
        a.download = `telemetry_${category}_${Date.now()}.csv`;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        window.URL.revokeObjectURL(downloadUrl);
    }

    /**
     * Get memory statistics
     */
    async getMemoryStats() {
        return await this.apiCall('/api/telemetry/stats/memory');
    }

    /**
     * Get sample count statistics
     */
    async getSampleStats() {
        return await this.apiCall('/api/telemetry/stats/samples');
    }

    /**
     * Get time range statistics
     */
    async getTimeRangeStats() {
        return await this.apiCall('/api/telemetry/stats/timerange');
    }

    /**
     * Get system health check
     */
    async getHealth() {
        return await this.apiCall('/api/health');
    }

    /**
     * Get API version
     */
    async getVersion() {
        return await this.apiCall('/api/version');
    }

    /**
     * Logout and clear session
     */
    async logout() {
        try {
            await this.apiCall('/api/auth/logout', { method: 'POST' });
        } catch (error) {
            console.error('Logout API call failed:', error);
        }
        
        this.disconnect();
        this.token = null;
        this.username = null;
        this.userRole = null;
        this.messageCount = 0;
        this.updateRate = 0;
        console.log('✓ Logged out');
    }

    /**
     * Get connection state
     */
    isConnected() {
        return this.ws && this.ws.readyState === WebSocket.OPEN;
    }

    /**
     * Get connection statistics
     */
    getStats() {
        return {
            messageCount: this.messageCount,
            updateRate: this.updateRate,
            connected: this.isConnected(),
            serverUrl: this.serverUrl,
            wsUrl: this.wsUrl,
            username: this.username,
            role: this.userRole
        };
    }
}

// Global telemetry client instance
const telemetryClient = new TelemetryClient();