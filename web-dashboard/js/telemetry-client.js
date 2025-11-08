/**
 * RCWS Telemetry Client
 * Handles WebSocket connections and REST API calls with JWT authentication
 */

class TelemetryClient {
    constructor() {
        this.serverUrl = '';
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

                if (this.onTelemetryUpdate) {
                    this.onTelemetryUpdate(message.data);
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
     */
    async getGimbalHistory(startTime, endTime) {
        const params = new URLSearchParams({
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/gimbal/history?${params}`);
    }

    /**
     * Get IMU data history
     */
    async getImuHistory(startTime, endTime) {
        const params = new URLSearchParams({
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/imu/history?${params}`);
    }

    /**
     * Get device status history
     */
    async getDeviceHistory(startTime, endTime) {
        const params = new URLSearchParams({
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/device/history?${params}`);
    }

    /**
     * Get tracking data history
     */
    async getTrackingHistory(startTime, endTime) {
        const params = new URLSearchParams({
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/tracking/history?${params}`);
    }

    /**
     * Get weapon status history
     */
    async getWeaponHistory(startTime, endTime) {
        const params = new URLSearchParams({
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });
        return await this.apiCall(`/api/telemetry/weapon/history?${params}`);
    }

    /**
     * Export telemetry data to CSV
     */
    async exportToCsv(category, startTime, endTime) {
        const params = new URLSearchParams({
            category,
            start: startTime.toISOString(),
            end: endTime.toISOString()
        });

        const url = `${this.serverUrl}/api/export/csv?${params}`;
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
     * Get system statistics
     */
    async getStatistics() {
        return await this.apiCall('/api/statistics');
    }

    /**
     * Logout and clear session
     */
    logout() {
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
