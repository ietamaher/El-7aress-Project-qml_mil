/**
 * RCWS Telemetry Dashboard
 * Main dashboard controller and UI logic
 */

class Dashboard {
    constructor() {
        this.currentView = 'live';
        this.lastTelemetryUpdate = null;
    }

    /**
     * Initialize the dashboard
     */
    init() {
        console.log('🚀 Initializing RCWS Telemetry Dashboard');

        // Show login modal
        this.showLoginModal();

        // Setup event listeners
        this.setupEventListeners();

        // Initialize charts
        telemetryCharts.initLiveChart('liveChart');
        telemetryCharts.initHistoricalChart('historicalChart');

        // Setup telemetry client callbacks
        telemetryClient.onConnected = () => this.onConnected();
        telemetryClient.onDisconnected = () => this.onDisconnected();
        telemetryClient.onTelemetryUpdate = (data) => this.onTelemetryUpdate(data);
        telemetryClient.onError = (error) => this.onError(error);

        console.log('✓ Dashboard initialized');
    }

    /**
     * Setup all event listeners
     */
    setupEventListeners() {
        // Login form
        const loginForm = document.getElementById('loginForm');
        loginForm.addEventListener('submit', (e) => this.handleLogin(e));

        // Logout button
        const logoutBtn = document.getElementById('logoutBtn');
        logoutBtn.addEventListener('click', () => this.handleLogout());

        // Navigation
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.addEventListener('click', () => this.switchView(item.dataset.view));
        });

        // Historical data controls
        const loadHistoricalBtn = document.getElementById('loadHistoricalBtn');
        loadHistoricalBtn.addEventListener('click', () => this.loadHistoricalData());

        const exportCsvBtn = document.getElementById('exportCsvBtn');
        exportCsvBtn.addEventListener('click', () => this.exportCsv());
    }

    /**
     * Show login modal
     */
    showLoginModal() {
        const modal = document.getElementById('loginModal');
        modal.classList.add('active');
    }

    /**
     * Hide login modal
     */
    hideLoginModal() {
        const modal = document.getElementById('loginModal');
        modal.classList.remove('active');
    }

    /**
     * Handle login form submission
     */
    async handleLogin(event) {
        event.preventDefault();

        const serverUrl = document.getElementById('serverUrl').value;
        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;
        const errorDiv = document.getElementById('loginError');

        errorDiv.textContent = '';

        try {
            console.log('Attempting login...');
            await telemetryClient.login(serverUrl, username, password);

            // Connect to WebSocket
            telemetryClient.connectWebSocket();

            // Update UI
            document.querySelector('.username').textContent = username;
            document.getElementById('logoutBtn').style.display = 'inline-block';

            this.hideLoginModal();

            console.log('✓ Login successful');
        } catch (error) {
            console.error('Login failed:', error);
            errorDiv.textContent = error.message || 'Login failed';
        }
    }

    /**
     * Handle logout
     */
    handleLogout() {
        telemetryClient.logout();
        this.updateConnectionStatus(false);
        document.querySelector('.username').textContent = 'Not logged in';
        document.getElementById('logoutBtn').style.display = 'none';
        telemetryCharts.clearCharts();
        this.showLoginModal();
        console.log('✓ Logged out');
    }

    /**
     * WebSocket connected callback
     */
    onConnected() {
        console.log('✓ Connected to telemetry stream');
        this.updateConnectionStatus(true);
        this.updateSystemInfo();

        // Start ping interval (every 20 seconds)
        this.pingInterval = setInterval(() => {
            telemetryClient.sendPing();
        }, 20000);
    }

    /**
     * WebSocket disconnected callback
     */
    onDisconnected() {
        console.log('✗ Disconnected from telemetry stream');
        this.updateConnectionStatus(false);

        if (this.pingInterval) {
            clearInterval(this.pingInterval);
            this.pingInterval = null;
        }
    }

    /**
     * Telemetry update callback
     */
    onTelemetryUpdate(telemetry) {
        this.lastTelemetryUpdate = telemetry;

        // Update live view
        this.updateGimbalDisplay(telemetry.gimbal);
        this.updateImuDisplay(telemetry.imu);
        this.updateDeviceDisplay(telemetry.device);
        this.updateWeaponDisplay(telemetry.weapon);
        this.updateTrackingDisplay(telemetry.tracking);

        // Update live chart
        telemetryCharts.updateLiveChart(telemetry);

        // Update connection info
        this.updateSystemInfo();
    }

    /**
     * Error callback
     */
    onError(error) {
        console.error('Telemetry error:', error);
    }

    /**
     * Update connection status indicator
     */
    updateConnectionStatus(connected) {
        const indicator = document.querySelector('.status-indicator');
        const text = document.querySelector('.status-text');

        if (connected) {
            indicator.classList.remove('offline');
            indicator.classList.add('online');
            text.textContent = 'Connected';
        } else {
            indicator.classList.remove('online');
            indicator.classList.add('offline');
            text.textContent = 'Disconnected';
        }
    }

    /**
     * Update gimbal display
     */
    updateGimbalDisplay(gimbal) {
        if (!gimbal) return;

        this.updateElement('gimbalAz', gimbal.gimbalAz, 2);
        this.updateElement('gimbalEl', gimbal.gimbalEl, 2);
        this.updateElement('azSpeed', gimbal.azSpeed, 2);
        this.updateElement('elSpeed', gimbal.elSpeed, 2);
    }

    /**
     * Update IMU display
     */
    updateImuDisplay(imu) {
        if (!imu) return;

        this.updateElement('imuRoll', imu.imuRollDeg, 2);
        this.updateElement('imuPitch', imu.imuPitchDeg, 2);
        this.updateElement('imuYaw', imu.imuYawDeg, 2);
        this.updateElement('imuTemp', imu.temperature, 1);
    }

    /**
     * Update device status display
     */
    updateDeviceDisplay(device) {
        if (!device) return;

        this.updateElement('azMotorTemp', device.azMotorTemp, 1);
        this.updateElement('azDriverTemp', device.azDriverTemp, 1);
        this.updateElement('elMotorTemp', device.elMotorTemp, 1);
        this.updateElement('elDriverTemp', device.elDriverTemp, 1);

        // Update temperature bars (0-100°C range)
        this.updateTempBar('azMotorTempBar', device.azMotorTemp);
        this.updateTempBar('azDriverTempBar', device.azDriverTemp);
        this.updateTempBar('elMotorTempBar', device.elMotorTemp);
        this.updateTempBar('elDriverTempBar', device.elDriverTemp);
    }

    /**
     * Update weapon status display
     */
    updateWeaponDisplay(weapon) {
        if (!weapon) return;

        this.updateStatusIndicator('gunArmed', weapon.gunArmed);
        this.updateStatusIndicator('ammoLoaded', weapon.ammoLoaded);
    }

    /**
     * Update tracking status display
     */
    updateTrackingDisplay(tracking) {
        if (!tracking) return;

        this.updateStatusIndicator('trackingActive', tracking.trackingActive);
        this.updateStatusIndicator('validTarget', tracking.hasValidTarget);
    }

    /**
     * Update element with formatted value
     */
    updateElement(id, value, decimals = 0) {
        const element = document.getElementById(id);
        if (element && value !== undefined) {
            element.textContent = typeof value === 'number'
                ? value.toFixed(decimals)
                : value;
        }
    }

    /**
     * Update temperature bar
     */
    updateTempBar(id, temp) {
        const bar = document.getElementById(id);
        if (bar && temp !== undefined) {
            const percentage = Math.min((temp / 100) * 100, 100);
            bar.style.width = `${percentage}%`;
        }
    }

    /**
     * Update status indicator light
     */
    updateStatusIndicator(id, active) {
        const indicator = document.querySelector(`#${id} .indicator-light`);
        if (indicator) {
            if (active) {
                indicator.classList.add('active');
            } else {
                indicator.classList.remove('active');
            }
        }
    }

    /**
     * Update system information panel
     */
    updateSystemInfo() {
        const stats = telemetryClient.getStats();

        document.getElementById('infoServerUrl').textContent = stats.serverUrl || '---';
        document.getElementById('infoWsUrl').textContent = stats.wsUrl || '---';
        document.getElementById('infoUsername').textContent = stats.username || '---';
        document.getElementById('infoRole').textContent = stats.role || '---';
        document.getElementById('infoConnectionState').textContent =
            stats.connected ? 'Connected' : 'Disconnected';
        document.getElementById('infoMessagesReceived').textContent = stats.messageCount;
        document.getElementById('infoUpdateRate').textContent =
            stats.updateRate ? `${stats.updateRate} Hz` : '--- Hz';
    }

    /**
     * Switch between views
     */
    switchView(viewName) {
        // Update navigation
        document.querySelectorAll('.nav-item').forEach(item => {
            item.classList.remove('active');
        });
        document.querySelector(`[data-view="${viewName}"]`).classList.add('active');

        // Update content
        document.querySelectorAll('.view-panel').forEach(panel => {
            panel.classList.remove('active');
        });
        document.getElementById(`${viewName}View`).classList.add('active');

        this.currentView = viewName;
        console.log(`Switched to ${viewName} view`);
    }

    /**
     * Load historical data
     */
    async loadHistoricalData() {
        const category = document.getElementById('dataCategory').value;
        const timeRange = parseInt(document.getElementById('timeRange').value);

        const endTime = new Date();
        const startTime = new Date(endTime.getTime() - timeRange * 60 * 60 * 1000);

        console.log(`Loading ${category} data from ${startTime.toISOString()} to ${endTime.toISOString()}`);

        try {
            let historyData;
            let stats;

            switch (category) {
                case 'gimbal':
                    historyData = await telemetryClient.getGimbalHistory(startTime, endTime);
                    stats = telemetryCharts.loadGimbalHistory(historyData);
                    break;
                case 'imu':
                    historyData = await telemetryClient.getImuHistory(startTime, endTime);
                    stats = telemetryCharts.loadImuHistory(historyData);
                    break;
                case 'device':
                    historyData = await telemetryClient.getDeviceHistory(startTime, endTime);
                    stats = telemetryCharts.loadDeviceHistory(historyData);
                    break;
                case 'tracking':
                    historyData = await telemetryClient.getTrackingHistory(startTime, endTime);
                    stats = telemetryCharts.loadGimbalHistory(historyData); // Reuse gimbal loader
                    break;
                case 'weapon':
                    historyData = await telemetryClient.getWeaponHistory(startTime, endTime);
                    stats = { samples: historyData.length, min: '---', max: '---', avg: '---' };
                    break;
            }

            // Update statistics
            if (stats) {
                document.getElementById('statSamples').textContent = stats.samples;
                document.getElementById('statMin').textContent = stats.min;
                document.getElementById('statMax').textContent = stats.max;
                document.getElementById('statAvg').textContent = stats.avg;
            }

            console.log(`✓ Loaded ${historyData.length} data points`);
        } catch (error) {
            console.error('Failed to load historical data:', error);
            alert('Failed to load historical data: ' + error.message);
        }
    }

    /**
     * Export telemetry data to CSV
     */
    async exportCsv() {
        const category = document.getElementById('dataCategory').value;
        const timeRange = parseInt(document.getElementById('timeRange').value);

        const endTime = new Date();
        const startTime = new Date(endTime.getTime() - timeRange * 60 * 60 * 1000);

        try {
            console.log(`Exporting ${category} data to CSV...`);
            await telemetryClient.exportToCsv(category, startTime, endTime);
            console.log('✓ CSV export complete');
        } catch (error) {
            console.error('CSV export failed:', error);
            alert('CSV export failed: ' + error.message);
        }
    }
}

// Initialize dashboard when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    const dashboard = new Dashboard();
    dashboard.init();
});
