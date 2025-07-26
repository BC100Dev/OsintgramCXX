#!/bin/bash

VCPKG_PREFIX_PATH="${1:-.vcpkg}"
BUILD_DIRECTORY="${2:-$(pwd)/Build/}"

function missing_tools() {
    echo "One or more tools are missing."
    echo "Using your package manager, install these following tools (example with apt):"
    echo "$ sudo apt install git curl zip unzip tar cmake libssl-dev libcurl4-openssl-dev"
    echo ""
    echo "You may have to adjust the packages tailored for your Linux OS,"
    echo "like for Arch Linux (using the pacman command):"
    echo "$ sudo pacman -Sy git base-devel zip unzip tar cmake"
    echo "- Note that I did not include curl here, since that package is required by pacman itself"
}

function check_vcpkg_bin() {
    if [ -n "$(command -v vcpkg)" ]; then
        return 0
    fi

    if [ -d "$VCPKG_PREFIX_PATH" ] && [ -f "$VCPKG_PREFIX_PATH/vcpkg" ] && [ -x "$VCPKG_PREFIX_PATH/vcpkg" ]; then
        return 0
    fi

    return 1
}

if check_vcpkg_bin; then
    echo "vcpkg already exists on your system."
    read -r -p "Do you wish to continue preparing vcpkg anyway? (Y/n) " VCPKG_INSTALL_CHECK

    if [[ "$VCPKG_INSTALL_CHECK" =~ ^[Nn]$ ]]; then
        echo "Cancelled."
        exit 0
    fi
fi

function clone_vcpkg() {
    "$1" clone https://github.com/microsoft/vcpkg.git "$2"
}

GIT_CMD="$(command -v git)"
CURL_CMD="$(command -v curl)"
ZIP_CMD="$(command -v zip)"
UNZIP_CMD="$(command -v unzip)"
TAR_CMD="$(command -v tar)"
CMAKE_CMD="$(command -v cmake)"

if [ -z "$CURL_CMD" ] || [ -z "$GIT_CMD" ] || [ -z "$ZIP_CMD" ] || [ -z "$UNZIP_CMD" ] || [ -z "$TAR_CMD" ] || [ -z "$CMAKE_CMD" ]; then
    missing_tools
    exit 1
fi

echo "Enabling explicit command error checks (don't attack me, if something does not get along with your expectations)"
set -e

echo ""
echo "VCPKG_PREPARE - Cloning vcpkg..."
if [ -d "$VCPKG_PREFIX_PATH" ]; then
    echo "Skipping clone; Verifying vcpkg repository structure"

    PROJ_ROOT="$(pwd)"
    cd "$VCPKG_PREFIX_PATH"

    if [ ! -d ".git" ]; then
        clone_vcpkg "$GIT_CMD" .
    else
        ORIGIN_OUT="$(${GIT_CMD} remote get-url origin)"
        if [[ "$ORIGIN_OUT" != https://github.com/microsoft/vcpkg* ]]; then
            echo "vcpkg update failed (not a valid vcpkg repository)"
            exit 1
        fi
    fi

    echo "Updating vcpkg..."
    "$GIT_CMD" fetch
    "$GIT_CMD" pull origin master

    cd "$PROJ_ROOT"
else
    clone_vcpkg "$GIT_CMD" "$VCPKG_PREFIX_PATH"
fi

echo ""
echo "VCPKG_PREPARE - Building vcpkg..."
"$SHELL" "$VCPKG_PREFIX_PATH/bootstrap-vcpkg.sh"

echo ""
echo "VCPKG_PREPARE - Installing curl with OpenSSL"
VCPKG_CMD="$VCPKG_PREFIX_PATH/vcpkg"

# for anyone wondering, this install command will also install OpenSSL and ZLIB, just to have this clarified
"$VCPKG_CMD" install curl\[openssl\]

echo ""
echo "BUILD_PREPARE - Preparing a build environment"
"$CMAKE_CMD" -DCMAKE_TOOLCHAIN_FILE="$(pwd)/.vcpkg/scripts/buildsystems/vcpkg.cmake" -S "$(pwd)" -B "$BUILD_DIRECTORY"

echo ""
echo "Build Preparation Complete."
echo "Run 'cmake --build $BUILD_DIRECTORY' to finish the job, and build the tool."