# Docker Deployment Guide - El 7aress RCWS

This guide covers Docker deployment for both **development (Ubuntu 22.04 with GTX 1650)** and **production (Jetson Orin AGX with Jetpack 6.2)** environments.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Development Environment (Ubuntu 22.04)](#development-environment-ubuntu-2204)
3. [Production Environment (Jetson Orin AGX)](#production-environment-jetson-orin-agx)
4. [Common Operations](#common-operations)
5. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Common Requirements

- Docker Engine 20.10+
- Docker Compose v2.0+
- NVIDIA Container Toolkit
- Git

### Development Machine (Ubuntu 22.04)

```bash
# Install Docker
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER

# Install Docker Compose
sudo apt-get update
sudo apt-get install docker-compose-plugin

# Install NVIDIA Container Toolkit
distribution=$(. /etc/os-release;echo $ID$VERSION_ID)
curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add -
curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | \
  sudo tee /etc/apt/sources.list.d/nvidia-docker.list

sudo apt-get update
sudo apt-get install -y nvidia-container-toolkit
sudo systemctl restart docker
```

### Jetson Orin AGX (Jetpack 6.2)

```bash
# Flash Jetpack 6.2 using NVIDIA SDK Manager first
# Then install Docker (usually pre-installed on Jetpack)

# Verify Docker installation
docker --version
docker-compose --version

# Verify NVIDIA runtime
docker run --rm --runtime nvidia nvcr.io/nvidia/l4t-base:r36.4.0 nvidia-smi
```

---

## Development Environment (Ubuntu 22.04)

### Configuration

**Dockerfile.dev** - Multi-stage build optimized for development:
- **Stage 0**: Builds OpenCV 4.10.0 with CUDA 12.2.2 for GTX 1650 (compute 7.5)
- **Stage 1**: Builds Qt6 application with VPI3 x86_64 packages
- **Stage 2**: Minimal runtime image

**docker-compose.dev.yml** - Development compose configuration:
- GPU support via NVIDIA runtime
- X11 forwarding for GUI display
- Volume mounts for live development
- Network host mode for hardware access

### Building

```bash
# Build the development image
docker-compose -f docker-compose.dev.yml build

# This will take 20-40 minutes on first build (OpenCV compilation)
# Subsequent builds use Docker layer caching and are much faster
```

### Running

```bash
# Allow X11 connections (needed for GUI)
xhost +local:docker

# Run the application
docker-compose -f docker-compose.dev.yml up

# Run in detached mode
docker-compose -f docker-compose.dev.yml up -d

# View logs
docker-compose -f docker-compose.dev.yml logs -f

# Stop the application
docker-compose -f docker-compose.dev.yml down
```

### Development Workflow

```bash
# 1. Make code changes in your editor

# 2. Rebuild only the application (uses cached OpenCV)
docker-compose -f docker-compose.dev.yml build --no-cache qt-app

# 3. Restart the container
docker-compose -f docker-compose.dev.yml up --force-recreate qt-app

# 4. Check logs
docker-compose -f docker-compose.dev.yml logs -f qt-app
```

### Environment Variables

Create a `.env` file in the project root:

```env
# Display
DISPLAY=:0

# GStreamer debug level (0=none, 1=error, 2=warning, 3=info, 4=debug)
GST_DEBUG=2

# YOLO model path (on host)
YOLO_MODEL_PATH=${HOME}/yolov8s.onnx
```

### Running with Simulation

The dev compose includes a Modbus simulation service for testing without hardware:

```bash
# Start with simulation profile
docker-compose -f docker-compose.dev.yml --profile simulation up
```

---

## Production Environment (Jetson Orin AGX)

### Configuration

**Dockerfile.prod** - Production build for ARM64:
- **Stage 0**: Builds OpenCV 4.10.0 with CUDA for Jetson Orin (compute 8.7 - Ampere)
- **Stage 1**: Builds Qt6 application using ARM64 packages
- **Stage 2**: Optimized runtime with Jetpack base

**docker-compose.prod.yml** - Production deployment:
- Native Jetson GPU support
- EGLFS platform (direct framebuffer, no X server needed)
- Health checks
- Auto-restart policy
- Resource limits

### Building on Jetson

```bash
# Clone the repository on Jetson
git clone <your-repo-url>
cd El-7aress-Project-qml_mil

# Build the production image
docker-compose -f docker-compose.prod.yml build

# This will take 30-60 minutes on first build (OpenCV compilation)
```

### Building on x86_64 (Cross-Compilation)

For faster builds, you can cross-compile on a powerful x86_64 machine:

```bash
# Install buildx
docker buildx create --name multiarch --use
docker buildx inspect --bootstrap

# Build for ARM64
docker buildx build \
  --platform linux/arm64 \
  -f Dockerfile.prod \
  -t el-7aress:prod-jetson \
  --load \
  .

# Save image
docker save el-7aress:prod-jetson | gzip > el-7aress-prod.tar.gz

# Transfer to Jetson
scp el-7aress-prod.tar.gz jetson@<jetson-ip>:~/

# On Jetson, load image
ssh jetson@<jetson-ip>
docker load < el-7aress-prod.tar.gz
```

### Running on Jetson

```bash
# Run in production mode
docker-compose -f docker-compose.prod.yml up -d

# Check status
docker-compose -f docker-compose.prod.yml ps

# View logs
docker-compose -f docker-compose.prod.yml logs -f

# Stop
docker-compose -f docker-compose.prod.yml down
```

### Display Modes

The production image supports two display modes:

#### 1. EGLFS (Default - Direct Framebuffer)

Best for dedicated systems without X server:

```yaml
environment:
  - QT_QPA_PLATFORM=eglfs
  - QT_QPA_EGLFS_ALWAYS_SET_MODE=1
  - QT_QPA_EGLFS_PHYSICAL_WIDTH=1920
  - QT_QPA_EGLFS_PHYSICAL_HEIGHT=1080
```

#### 2. XCB (X11 Display Server)

If you have an X server running:

```yaml
environment:
  - DISPLAY=${DISPLAY:-:0}
  - QT_QPA_PLATFORM=xcb

volumes:
  - /tmp/.X11-unix:/tmp/.X11-unix:rw
```

Then run: `xhost +local:docker` before starting.

### Systemd Service (Auto-start on Boot)

Create `/etc/systemd/system/rcws.service`:

```ini
[Unit]
Description=El 7aress RCWS Docker Container
After=docker.service
Requires=docker.service

[Service]
Type=oneshot
RemainAfterExit=yes
WorkingDirectory=/opt/rcws
ExecStart=/usr/bin/docker-compose -f docker-compose.prod.yml up -d
ExecStop=/usr/bin/docker-compose -f docker-compose.prod.yml down
TimeoutStartSec=0

[Install]
WantedBy=multi-user.target
```

Enable and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable rcws.service
sudo systemctl start rcws.service
sudo systemctl status rcws.service
```

---

## Common Operations

### View Container Status

```bash
# Development
docker-compose -f docker-compose.dev.yml ps

# Production
docker-compose -f docker-compose.prod.yml ps
```

### Access Container Shell

```bash
# Development
docker-compose -f docker-compose.dev.yml exec qt-app /bin/bash

# Production
docker-compose -f docker-compose.prod.yml exec rcws-app /bin/bash
```

### View Real-time Logs

```bash
# All logs
docker-compose -f docker-compose.dev.yml logs -f

# Specific service
docker-compose -f docker-compose.dev.yml logs -f qt-app

# Last 100 lines
docker-compose -f docker-compose.dev.yml logs --tail=100 qt-app
```

### Restart Service

```bash
# Development
docker-compose -f docker-compose.dev.yml restart qt-app

# Production
docker-compose -f docker-compose.prod.yml restart rcws-app
```

### Clean Up

```bash
# Stop and remove containers
docker-compose -f docker-compose.dev.yml down

# Stop and remove containers + volumes
docker-compose -f docker-compose.dev.yml down -v

# Remove all images
docker-compose -f docker-compose.dev.yml down --rmi all

# Clean Docker system (careful!)
docker system prune -a
```

### Update Application

```bash
# 1. Pull latest code
git pull origin main

# 2. Rebuild
docker-compose -f docker-compose.prod.yml build --no-cache

# 3. Restart
docker-compose -f docker-compose.prod.yml up -d --force-recreate
```

---

## Troubleshooting

### Issue: GPU Not Detected

**Symptoms**: CUDA errors, VPI initialization fails

**Solution**:
```bash
# Verify NVIDIA runtime
docker run --rm --runtime nvidia nvidia/cuda:12.2.2-base-ubuntu22.04 nvidia-smi

# Check nvidia-container-toolkit
sudo systemctl status nvidia-container-toolkit

# Restart Docker
sudo systemctl restart docker
```

### Issue: No Video Display

**Symptoms**: Black screen, "Cannot connect to X server"

**Solution**:
```bash
# Allow X11 connections
xhost +local:docker

# Check DISPLAY variable
echo $DISPLAY

# Try different QT platform
# Edit docker-compose and change QT_QPA_PLATFORM to 'xcb' or 'eglfs'
```

### Issue: Serial Devices Not Found

**Symptoms**: "Cannot open /dev/serial/by-id/..."

**Solution**:
```bash
# Check devices on host
ls -la /dev/serial/by-id/

# Verify user is in dialout group
groups $USER

# Add to dialout if needed
sudo usermod -aG dialout $USER
# Then log out and back in

# Check container has access
docker-compose exec qt-app ls -la /dev/serial/by-id/
```

### Issue: Build Fails (OpenCV)

**Symptoms**: cmake errors, missing CUDA

**Solution**:
```bash
# Ensure CUDA is installed
nvidia-smi

# Check CUDA version matches Dockerfile
# For dev: CUDA 12.2.2
# For prod: Jetpack 6.2 includes CUDA

# Increase Docker build memory
# Edit /etc/docker/daemon.json:
{
  "default-runtime": "nvidia",
  "runtimes": {
    "nvidia": {
      "path": "nvidia-container-runtime",
      "runtimeArgs": []
    }
  },
  "default-ulimits": {
    "memlock": {
      "Hard": -1,
      "Name": "memlock",
      "Soft": -1
    }
  }
}

sudo systemctl restart docker
```

### Issue: Container Crashes on Startup

**Symptoms**: Container exits immediately

**Solution**:
```bash
# Check logs
docker-compose logs qt-app

# Run container interactively
docker run -it --rm --runtime nvidia \
  --entrypoint /bin/bash \
  el-7aress:dev

# Inside container, test manually
./QT6-gstreamer-example
```

### Issue: Network API Not Accessible

**Symptoms**: Cannot reach telemetry API

**Solution**:
```bash
# Check if using host network
# In docker-compose.yml, ensure: network_mode: host

# Check port bindings
docker port <container-name>

# Test API
curl http://localhost:8443/api/health

# Check firewall
sudo ufw status
sudo ufw allow 8443/tcp
```

### Issue: Qt Platform Plugin Error

**Symptoms**: "Could not load the Qt platform plugin 'xcb'"

**Solution**:
```bash
# Install missing X11 libraries in container
# Add to Dockerfile:
RUN apt-get install -y libxcb-xinerama0

# Or switch to EGLFS platform
# In docker-compose.yml:
environment:
  - QT_QPA_PLATFORM=eglfs
```

---

## Performance Optimization

### Build Performance

```bash
# Use build cache
docker-compose build

# Parallel builds
docker-compose build --parallel

# Build specific stage only
docker build --target builder -f Dockerfile.dev .
```

### Runtime Performance

```yaml
# Add resource limits in docker-compose.yml
deploy:
  resources:
    limits:
      cpus: '6'
      memory: 8G
    reservations:
      cpus: '4'
      memory: 4G
```

### Storage Optimization

```bash
# Remove unused images
docker image prune -a

# Remove build cache
docker builder prune -a

# Check disk usage
docker system df
```

---

## Security Considerations

### Production Deployment

1. **Change default credentials** in telemetry API
2. **Enable TLS/SSL** for API endpoints
3. **Configure IP whitelisting** for telemetry access
4. **Use secrets management** for sensitive data
5. **Regular security updates**:
   ```bash
   # Update base images
   docker pull nvcr.io/nvidia/l4t-jetpack:r36.4.0

   # Rebuild with latest security patches
   docker-compose -f docker-compose.prod.yml build --pull --no-cache
   ```

### Network Security

```yaml
# Use custom bridge network instead of host
networks:
  rcws-net:
    driver: bridge
    ipam:
      config:
        - subnet: 172.20.0.0/24

services:
  rcws-app:
    networks:
      - rcws-net
    ports:
      - "8443:8443"  # Expose only needed ports
```

---

## Additional Resources

- **Qt6 Documentation**: https://doc.qt.io/qt-6/
- **NVIDIA Jetpack**: https://developer.nvidia.com/embedded/jetpack
- **Docker Documentation**: https://docs.docker.com/
- **GStreamer Guide**: https://gstreamer.freedesktop.org/documentation/

---

## Support

For technical support, please contact through official military channels or refer to the main README.md file.

---

**Last Updated**: January 2025
**Version**: 1.0
