#!/bin/bash
# =============================================================================
# Docker Build and Run Script for El 7aress RCWS
# =============================================================================
#
# This script simplifies building and running the application in Docker
# for both development (Ubuntu 22.04) and production (Jetson Orin AGX)
#
# Usage:
#   ./docker-build.sh dev build     - Build development image
#   ./docker-build.sh dev run       - Run development container
#   ./docker-build.sh prod build    - Build production image
#   ./docker-build.sh prod run      - Run production container
# =============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Function to print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check prerequisites
check_prerequisites() {
    print_info "Checking prerequisites..."

    # Check Docker
    if ! command -v docker &> /dev/null; then
        print_error "Docker is not installed!"
        exit 1
    fi
    print_success "Docker found: $(docker --version)"

    # Check Docker Compose
    if ! docker compose version &> /dev/null; then
        print_error "Docker Compose is not installed!"
        exit 1
    fi
    print_success "Docker Compose found: $(docker compose version)"

    # Check NVIDIA runtime
    if ! docker run --rm --runtime nvidia nvidia/cuda:12.2.2-base-ubuntu22.04 nvidia-smi &> /dev/null; then
        print_warning "NVIDIA runtime not available or GPU not accessible"
        print_warning "GPU acceleration may not work!"
    else
        print_success "NVIDIA runtime is available"
    fi

    # Check if in docker group
    if ! groups | grep -q docker; then
        print_warning "Current user is not in 'docker' group"
        print_warning "You may need to run with sudo or add user to docker group"
    fi
}

# Function to build development image
build_dev() {
    print_info "Building development image for Ubuntu 22.04 with GTX 1650..."
    print_info "This will take 20-40 minutes on first build (compiling OpenCV with CUDA)"

    # Allow X11 connections for GUI
    xhost +local:docker 2>/dev/null || print_warning "Could not configure X11 (xhost not found)"

    # Build
    docker compose -f docker-compose.dev.yml build "$@"

    print_success "Development image built successfully!"
    print_info "Image name: el-7aress:dev"
}

# Function to run development container
run_dev() {
    print_info "Starting development container..."

    # Allow X11 connections
    xhost +local:docker 2>/dev/null || print_warning "Could not configure X11 (xhost not found)"

    # Check if YOLO model exists
    if [ ! -f "$HOME/yolov8s.onnx" ]; then
        print_warning "YOLO model not found at $HOME/yolov8s.onnx"
        print_info "Detection features will not work without the model"
    fi

    # Run
    docker compose -f docker-compose.dev.yml up "$@"
}

# Function to build production image
build_prod() {
    print_info "Building production image for Jetson Orin AGX (Jetpack 6.2)..."
    print_info "This will take 30-60 minutes on first build (compiling OpenCV with CUDA for ARM64)"

    # Check if we're on ARM64
    ARCH=$(uname -m)
    if [ "$ARCH" != "aarch64" ] && [ "$ARCH" != "arm64" ]; then
        print_warning "Not running on ARM64 architecture (detected: $ARCH)"
        print_warning "This build is intended for Jetson Orin AGX"
        read -p "Continue anyway? (y/N) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "Build cancelled"
            exit 0
        fi
    fi

    # Build
    docker compose -f docker-compose.prod.yml build "$@"

    print_success "Production image built successfully!"
    print_info "Image name: el-7aress:prod-jetson"
}

# Function to run production container
run_prod() {
    print_info "Starting production container..."

    # Check if YOLO model exists
    if [ ! -f "$HOME/yolov8s.onnx" ]; then
        print_warning "YOLO model not found at $HOME/yolov8s.onnx"
        print_info "Detection features will not work without the model"
    fi

    # Check if zones.json exists
    if [ ! -f "$SCRIPT_DIR/config/zones.json" ]; then
        print_warning "zones.json not found at $SCRIPT_DIR/config/zones.json"
    fi

    # Run
    docker compose -f docker-compose.prod.yml up "$@"
}

# Function to stop containers
stop_containers() {
    local ENV=$1
    if [ "$ENV" = "dev" ]; then
        print_info "Stopping development containers..."
        docker compose -f docker-compose.dev.yml down
    else
        print_info "Stopping production containers..."
        docker compose -f docker-compose.prod.yml down
    fi
    print_success "Containers stopped"
}

# Function to show logs
show_logs() {
    local ENV=$1
    shift
    if [ "$ENV" = "dev" ]; then
        docker compose -f docker-compose.dev.yml logs "$@"
    else
        docker compose -f docker-compose.prod.yml logs "$@"
    fi
}

# Function to clean up
cleanup() {
    local ENV=$1
    print_warning "This will remove all containers, networks, and volumes"
    read -p "Are you sure? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "Cleanup cancelled"
        exit 0
    fi

    if [ "$ENV" = "dev" ]; then
        docker compose -f docker-compose.dev.yml down -v --rmi all
    else
        docker compose -f docker-compose.prod.yml down -v --rmi all
    fi
    print_success "Cleanup complete"
}

# Function to show status
show_status() {
    local ENV=$1
    print_info "Container status:"
    if [ "$ENV" = "dev" ]; then
        docker compose -f docker-compose.dev.yml ps
    else
        docker compose -f docker-compose.prod.yml ps
    fi
}

# Function to show help
show_help() {
    cat << EOF
${BLUE}El 7aress RCWS - Docker Build and Run Script${NC}

Usage: ./docker-build.sh <environment> <command> [options]

Environments:
  ${GREEN}dev${NC}      Development (Ubuntu 22.04 with GTX 1650)
  ${GREEN}prod${NC}     Production (Jetson Orin AGX with Jetpack 6.2)

Commands:
  ${GREEN}build${NC}    Build Docker image
  ${GREEN}run${NC}      Run container (add -d for detached mode)
  ${GREEN}stop${NC}     Stop running containers
  ${GREEN}logs${NC}     Show container logs (add -f to follow)
  ${GREEN}status${NC}   Show container status
  ${GREEN}shell${NC}    Open shell in running container
  ${GREEN}clean${NC}    Remove containers, networks, and images
  ${GREEN}help${NC}     Show this help message

Examples:
  ${YELLOW}# Build development image${NC}
  ./docker-build.sh dev build

  ${YELLOW}# Run development container in foreground${NC}
  ./docker-build.sh dev run

  ${YELLOW}# Run development container in background${NC}
  ./docker-build.sh dev run -d

  ${YELLOW}# Show development logs${NC}
  ./docker-build.sh dev logs -f

  ${YELLOW}# Build production image${NC}
  ./docker-build.sh prod build

  ${YELLOW}# Run production container${NC}
  ./docker-build.sh prod run -d

  ${YELLOW}# Stop production containers${NC}
  ./docker-build.sh prod stop

  ${YELLOW}# Show container status${NC}
  ./docker-build.sh prod status

  ${YELLOW}# Open shell in running dev container${NC}
  ./docker-build.sh dev shell

For more information, see DOCKER_DEPLOYMENT.md

EOF
}

# Function to open shell
open_shell() {
    local ENV=$1
    if [ "$ENV" = "dev" ]; then
        docker compose -f docker-compose.dev.yml exec qt-app /bin/bash
    else
        docker compose -f docker-compose.prod.yml exec rcws-app /bin/bash
    fi
}

# Main script logic
main() {
    # Check arguments
    if [ $# -lt 1 ]; then
        show_help
        exit 1
    fi

    ENV=$1
    COMMAND=$2
    shift 2 || true

    # Validate environment
    if [ "$ENV" != "dev" ] && [ "$ENV" != "prod" ]; then
        print_error "Invalid environment: $ENV"
        print_info "Valid environments: dev, prod"
        exit 1
    fi

    # Execute command
    case "$COMMAND" in
        build)
            check_prerequisites
            if [ "$ENV" = "dev" ]; then
                build_dev "$@"
            else
                build_prod "$@"
            fi
            ;;
        run)
            check_prerequisites
            if [ "$ENV" = "dev" ]; then
                run_dev "$@"
            else
                run_prod "$@"
            fi
            ;;
        stop)
            stop_containers "$ENV"
            ;;
        logs)
            show_logs "$ENV" "$@"
            ;;
        status)
            show_status "$ENV"
            ;;
        shell)
            open_shell "$ENV"
            ;;
        clean)
            cleanup "$ENV"
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            print_error "Unknown command: $COMMAND"
            show_help
            exit 1
            ;;
    esac
}

# Run main
main "$@"
