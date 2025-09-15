#!/bin/bash

# MasterBot Build Script
# ======================
# 
# DEPENDENCIES:
# - CMake (>= 3.16)
# - C++ Compiler (GCC >= 7 or Clang >= 5)
# - libcurl4-openssl-dev / curl-devel
# - nlohmann-json3-dev / json-devel
# - pkg-config
# - make
# - git (for downloading nlohmann/json if not available)
# 
# OPTIONAL DEPENDENCIES (for testing):
# - libgtest-dev / gtest-devel
# - cmake-gtest (some distros)

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
INSTALL_DEPS=false
BUILD_TESTS=false
VERBOSE=false

# Print colored output
print_color() {
    printf "${1}${2}${NC}\n"
}

print_header() {
    echo "=================================="
    print_color $BLUE "$1"
    echo "=================================="
}

print_success() {
    print_color $GREEN "✓ $1"
}

print_warning() {
    print_color $YELLOW "⚠ $1"
}

print_error() {
    print_color $RED "✗ $1"
}

# Show usage information
show_usage() {
    echo "MasterBot Build Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -d, --deps          Install dependencies automatically"
    echo "  -t, --tests         Build with tests enabled"
    echo "  -v, --verbose       Verbose build output"
    echo "  -c, --clean         Clean build directory before building"
    echo "  -h, --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --deps           # Install deps and build"
    echo "  $0 --deps --tests   # Install deps, build with tests"
    echo "  $0 --clean          # Clean build and rebuild"
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -d|--deps)
                INSTALL_DEPS=true
                shift
                ;;
            -t|--tests)
                BUILD_TESTS=true
                shift
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -c|--clean)
                if [ -d "$BUILD_DIR" ]; then
                    print_warning "Cleaning build directory..."
                    rm -rf "$BUILD_DIR"
                fi
                shift
                ;;
            -h|--help)
                show_usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
        DISTRO_ID=$ID
        VERSION_ID=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        OS=$(cat /etc/redhat-release)
        DISTRO_ID="rhel"
    elif [ -f /etc/debian_version ]; then
        OS="Debian"
        DISTRO_ID="debian"
    else
        OS=$(uname -s)
        DISTRO_ID="unknown"
    fi
    
    print_success "Detected OS: $OS"
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install dependencies for Ubuntu/Debian
install_deps_ubuntu() {
    print_header "Installing dependencies for Ubuntu/Debian"
    
    sudo apt-get update
    
    # Essential build tools
    sudo apt-get install -y \
        build-essential \
        cmake \
        pkg-config \
        git
    
    # Main dependencies
    sudo apt-get install -y \
        libcurl4-openssl-dev \
        nlohmann-json3-dev
    
    # Optional test dependencies
    if [ "$BUILD_TESTS" = true ]; then
        sudo apt-get install -y \
            libgtest-dev \
            libgmock-dev
    fi
    
    print_success "Dependencies installed for Ubuntu/Debian"
}

# Install dependencies for CentOS/RHEL/Fedora
install_deps_redhat() {
    print_header "Installing dependencies for CentOS/RHEL/Fedora"
    
    # Detect package manager
    if command_exists dnf; then
        PKG_MGR="dnf"
    elif command_exists yum; then
        PKG_MGR="yum"
    else
        print_error "No package manager found (dnf/yum)"
        exit 1
    fi
    
    # Essential build tools
    sudo $PKG_MGR install -y \
        gcc-c++ \
        cmake \
        make \
        pkg-config \
        git
    
    # Main dependencies
    sudo $PKG_MGR install -y \
        libcurl-devel
    
    # Try to install nlohmann-json, fallback to manual install
    if ! sudo $PKG_MGR install -y json-devel 2>/dev/null; then
        print_warning "nlohmann-json not available, will download manually"
        download_nlohmann_json
    fi
    
    # Optional test dependencies
    if [ "$BUILD_TESTS" = true ]; then
        sudo $PKG_MGR install -y gtest-devel gmock-devel 2>/dev/null || \
            print_warning "GTest not available in repositories"
    fi
    
    print_success "Dependencies installed for CentOS/RHEL/Fedora"
}

# Install dependencies for Arch Linux
install_deps_arch() {
    print_header "Installing dependencies for Arch Linux"
    
    sudo pacman -Sy --noconfirm \
        base-devel \
        cmake \
        curl \
        nlohmann-json \
        pkg-config
    
    # Optional test dependencies
    if [ "$BUILD_TESTS" = true ]; then
        sudo pacman -S --noconfirm gtest gmock
    fi
    
    print_success "Dependencies installed for Arch Linux"
}

# Install dependencies for openSUSE
install_deps_opensuse() {
    print_header "Installing dependencies for openSUSE"
    
    sudo zypper install -y \
        gcc-c++ \
        cmake \
        make \
        pkg-config \
        git \
        libcurl-devel \
        nlohmann_json-devel
    
    # Optional test dependencies
    if [ "$BUILD_TESTS" = true ]; then
        sudo zypper install -y gtest gmock
    fi
    
    print_success "Dependencies installed for openSUSE"
}

# Download and setup nlohmann-json manually
download_nlohmann_json() {
    print_warning "Downloading nlohmann-json manually..."
    
    NLOHMANN_VERSION="v3.11.2"
    NLOHMANN_DIR="third_party/nlohmann"
    
    mkdir -p "$NLOHMANN_DIR"
    
    if [ ! -f "$NLOHMANN_DIR/json.hpp" ]; then
        curl -L "https://github.com/nlohmann/json/releases/download/$NLOHMANN_VERSION/json.hpp" \
            -o "$NLOHMANN_DIR/json.hpp"
        print_success "Downloaded nlohmann-json header"
    fi
}

# Install dependencies based on distribution
install_dependencies() {
    if [ "$INSTALL_DEPS" = false ]; then
        return
    fi
    
    case $DISTRO_ID in
        ubuntu|debian)
            install_deps_ubuntu
            ;;
        rhel|centos|fedora|rocky|almalinux)
            install_deps_redhat
            ;;
        arch|manjaro)
            install_deps_arch
            ;;
        opensuse*|sled|sles)
            install_deps_opensuse
            ;;
        *)
            print_warning "Unsupported distribution: $DISTRO_ID"
            print_warning "Please install dependencies manually:"
            echo "  - CMake (>= 3.16)"
            echo "  - C++ compiler (GCC >= 7 or Clang >= 5)"
            echo "  - libcurl development headers"
            echo "  - nlohmann-json development headers"
            echo "  - pkg-config"
            echo ""
            read -p "Continue anyway? (y/N): " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                exit 1
            fi
            ;;
    esac
}

# Check for required tools
check_requirements() {
    print_header "Checking build requirements"
    
    local missing_tools=()
    
    if ! command_exists cmake; then
        missing_tools+=("cmake")
    fi
    
    if ! command_exists make; then
        missing_tools+=("make")
    fi
    
    if ! command_exists pkg-config; then
        missing_tools+=("pkg-config")
    fi
    
    # Check for C++ compiler
    if ! command_exists g++ && ! command_exists clang++; then
        missing_tools+=("g++ or clang++")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        if [ "$INSTALL_DEPS" = false ]; then
            echo "Run with --deps to install dependencies automatically"
        fi
        exit 1
    fi
    
    print_success "All required tools found"
}

# Configure build
configure_build() {
    print_header "Configuring build"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    
    if [ "$BUILD_TESTS" = true ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DBUILD_TESTING=ON"
    fi
    
    if [ "$VERBOSE" = true ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
    fi
    
    # Add manual nlohmann-json path if it exists
    if [ -f "../third_party/nlohmann/json.hpp" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -Dnlohmann_json_DIR=../third_party"
    fi
    
    cmake .. $CMAKE_ARGS
    
    print_success "Build configured"
}

# Build project
build_project() {
    print_header "Building project"
    
    cd "$BUILD_DIR"
    
    MAKE_ARGS=""
    if [ "$VERBOSE" = true ]; then
        MAKE_ARGS="VERBOSE=1"
    fi
    
    # Use all available cores for compilation
    CORES=$(nproc 2>/dev/null || echo 4)
    make -j$CORES $MAKE_ARGS
    
    print_success "Build completed"
}

# Run tests if enabled
run_tests() {
    if [ "$BUILD_TESTS" = false ]; then
        return
    fi
    
    print_header "Running tests"
    
    cd "$BUILD_DIR"
    
    if [ -f "masterbot_tests" ]; then
        ./masterbot_tests
        print_success "All tests passed"
    else
        print_warning "Test executable not found, skipping tests"
    fi
}

# Show build summary
show_summary() {
    print_header "Build Summary"
    
    echo "OS: $OS"
    echo "Build directory: $BUILD_DIR"
    echo "Tests enabled: $BUILD_TESTS"
    
    if [ -f "$BUILD_DIR/masterbot" ]; then
        print_success "Executable: $BUILD_DIR/masterbot"
        echo ""
        echo "To run the application:"
        echo "  cd $BUILD_DIR && ./masterbot"
    else
        print_error "Build failed - executable not found"
        exit 1
    fi
}

# Main execution
main() {
    print_header "MasterBot Build Script"
    
    parse_args "$@"
    detect_distro
    install_dependencies
    check_requirements
    configure_build
    build_project
    run_tests
    show_summary
    
    print_success "Build script completed successfully!"
}

# Run main function with all arguments
main "$@"