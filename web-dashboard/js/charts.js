/**
 * RCWS Telemetry Charts
 * Chart.js integration for live and historical data visualization
 */

class TelemetryCharts {
    constructor() {
        this.liveChart = null;
        this.historicalChart = null;
        this.liveDataPoints = {
            labels: [],
            azimuth: [],
            elevation: []
        };
        this.maxLiveDataPoints = 300; // 30 seconds at 10 Hz
    }

    /**
     * Initialize live telemetry chart (gimbal position)
     */
    initLiveChart(canvasId) {
        const ctx = document.getElementById(canvasId).getContext('2d');

        this.liveChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: this.liveDataPoints.labels,
                datasets: [
                    {
                        label: 'Azimuth (°)',
                        data: this.liveDataPoints.azimuth,
                        borderColor: '#06b6d4',
                        backgroundColor: 'rgba(6, 182, 212, 0.1)',
                        borderWidth: 2,
                        pointRadius: 0,
                        tension: 0.4
                    },
                    {
                        label: 'Elevation (°)',
                        data: this.liveDataPoints.elevation,
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.1)',
                        borderWidth: 2,
                        pointRadius: 0,
                        tension: 0.4
                    }
                ]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                animation: false, // Disable for real-time performance
                interaction: {
                    intersect: false,
                    mode: 'index'
                },
                plugins: {
                    legend: {
                        display: true,
                        labels: {
                            color: '#e6e9ef',
                            font: {
                                size: 12
                            }
                        }
                    },
                    tooltip: {
                        backgroundColor: 'rgba(26, 33, 40, 0.9)',
                        titleColor: '#e6e9ef',
                        bodyColor: '#e6e9ef',
                        borderColor: '#2d3748',
                        borderWidth: 1
                    }
                },
                scales: {
                    x: {
                        display: true,
                        grid: {
                            color: '#2d3748'
                        },
                        ticks: {
                            color: '#a0a8b8',
                            maxRotation: 0,
                            autoSkip: true,
                            maxTicksLimit: 10
                        }
                    },
                    y: {
                        display: true,
                        grid: {
                            color: '#2d3748'
                        },
                        ticks: {
                            color: '#a0a8b8'
                        }
                    }
                }
            }
        });

        console.log('✓ Live chart initialized');
    }

    /**
     * Update live chart with new telemetry data
     */
    updateLiveChart(telemetry) {
        if (!this.liveChart || !telemetry.gimbal) return;

        const now = new Date();
        const timeLabel = now.toLocaleTimeString('en-US', {
            hour12: false,
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        });

        // Add new data point
        this.liveDataPoints.labels.push(timeLabel);
        this.liveDataPoints.azimuth.push(telemetry.gimbal.gimbalAz || 0);
        this.liveDataPoints.elevation.push(telemetry.gimbal.gimbalEl || 0);

        // Remove old data points (keep last 30 seconds)
        if (this.liveDataPoints.labels.length > this.maxLiveDataPoints) {
            this.liveDataPoints.labels.shift();
            this.liveDataPoints.azimuth.shift();
            this.liveDataPoints.elevation.shift();
        }

        // Update chart
        this.liveChart.update('none'); // 'none' mode for better performance
    }

    /**
     * Initialize historical data chart
     */
    initHistoricalChart(canvasId) {
        const ctx = document.getElementById(canvasId).getContext('2d');

        this.historicalChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: []
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction: {
                    intersect: false,
                    mode: 'index'
                },
                plugins: {
                    legend: {
                        display: true,
                        labels: {
                            color: '#e6e9ef',
                            font: {
                                size: 12
                            }
                        }
                    },
                    tooltip: {
                        backgroundColor: 'rgba(26, 33, 40, 0.9)',
                        titleColor: '#e6e9ef',
                        bodyColor: '#e6e9ef',
                        borderColor: '#2d3748',
                        borderWidth: 1
                    },
                    zoom: {
                        zoom: {
                            wheel: {
                                enabled: true
                            },
                            pinch: {
                                enabled: true
                            },
                            mode: 'x'
                        },
                        pan: {
                            enabled: true,
                            mode: 'x'
                        }
                    }
                },
                scales: {
                    x: {
                        display: true,
                        grid: {
                            color: '#2d3748'
                        },
                        ticks: {
                            color: '#a0a8b8',
                            maxRotation: 45,
                            autoSkip: true,
                            maxTicksLimit: 15
                        }
                    },
                    y: {
                        display: true,
                        grid: {
                            color: '#2d3748'
                        },
                        ticks: {
                            color: '#a0a8b8'
                        }
                    }
                }
            }
        });

        console.log('✓ Historical chart initialized');
    }

    /**
     * Load gimbal history data into chart
     */
    loadGimbalHistory(historyData) {
        if (!this.historicalChart) return;

        const labels = historyData.map(point => {
            const date = new Date(point.timestamp);
            return date.toLocaleString('en-US', {
                month: 'short',
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit',
                hour12: false
            });
        });

        const azData = historyData.map(point => point.gimbalAz);
        const elData = historyData.map(point => point.gimbalEl);

        this.historicalChart.data.labels = labels;
        this.historicalChart.data.datasets = [
            {
                label: 'Azimuth (°)',
                data: azData,
                borderColor: '#06b6d4',
                backgroundColor: 'rgba(6, 182, 212, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            },
            {
                label: 'Elevation (°)',
                data: elData,
                borderColor: '#10b981',
                backgroundColor: 'rgba(16, 185, 129, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            }
        ];

        this.historicalChart.update();
        console.log(`✓ Loaded ${historyData.length} gimbal data points`);

        return this.calculateStatistics([...azData, ...elData]);
    }

    /**
     * Load IMU history data into chart
     */
    loadImuHistory(historyData) {
        if (!this.historicalChart) return;

        const labels = historyData.map(point => {
            const date = new Date(point.timestamp);
            return date.toLocaleString('en-US', {
                month: 'short',
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit',
                hour12: false
            });
        });

        const rollData = historyData.map(point => point.roll || point.imuRollDeg || 0);
        const pitchData = historyData.map(point => point.pitch || point.imuPitchDeg || 0);
        const yawData = historyData.map(point => point.yaw || point.imuYawDeg || 0);

        this.historicalChart.data.labels = labels;
        this.historicalChart.data.datasets = [
            {
                label: 'Roll (°)',
                data: rollData,
                borderColor: '#06b6d4',
                backgroundColor: 'rgba(6, 182, 212, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            },
            {
                label: 'Pitch (°)',
                data: pitchData,
                borderColor: '#10b981',
                backgroundColor: 'rgba(16, 185, 129, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            },
            {
                label: 'Yaw (°)',
                data: yawData,
                borderColor: '#f59e0b',
                backgroundColor: 'rgba(245, 158, 11, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            }
        ];

        this.historicalChart.update();
        console.log(`✓ Loaded ${historyData.length} IMU data points`);

        return this.calculateStatistics([...rollData, ...pitchData, ...yawData]);
    }

    /**
     * Load device status history into chart
     */
    loadDeviceHistory(historyData) {
        if (!this.historicalChart) return;

        const labels = historyData.map(point => {
            const date = new Date(point.timestamp);
            return date.toLocaleString('en-US', {
                month: 'short',
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                hour12: false
            });
        });

        const azMotorData = historyData.map(point => point.azMotorTemp);
        const elMotorData = historyData.map(point => point.elMotorTemp);

        this.historicalChart.data.labels = labels;
        this.historicalChart.data.datasets = [
            {
                label: 'Az Motor Temp (°C)',
                data: azMotorData,
                borderColor: '#ef4444',
                backgroundColor: 'rgba(239, 68, 68, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            },
            {
                label: 'El Motor Temp (°C)',
                data: elMotorData,
                borderColor: '#f59e0b',
                backgroundColor: 'rgba(245, 158, 11, 0.1)',
                borderWidth: 2,
                pointRadius: 1,
                tension: 0.4
            }
        ];

        this.historicalChart.update();
        console.log(`✓ Loaded ${historyData.length} device status points`);

        return this.calculateStatistics([...azMotorData, ...elMotorData]);
    }

    /**
     * Calculate statistics for dataset
     */
    calculateStatistics(data) {
        if (!data || data.length === 0) {
            return { samples: 0, min: 0, max: 0, avg: 0 };
        }

        const samples = data.length;
        const min = Math.min(...data);
        const max = Math.max(...data);
        const avg = data.reduce((a, b) => a + b, 0) / samples;

        return {
            samples,
            min: min.toFixed(2),
            max: max.toFixed(2),
            avg: avg.toFixed(2)
        };
    }

    /**
     * Clear all charts
     */
    clearCharts() {
        if (this.liveChart) {
            this.liveDataPoints.labels = [];
            this.liveDataPoints.azimuth = [];
            this.liveDataPoints.elevation = [];
            this.liveChart.update();
        }

        if (this.historicalChart) {
            this.historicalChart.data.labels = [];
            this.historicalChart.data.datasets = [];
            this.historicalChart.update();
        }
    }
}

// Global charts instance
const telemetryCharts = new TelemetryCharts();
