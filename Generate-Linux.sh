#!/bin/bash

PREMAKE_URL='https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-linux.tar.gz'
PREMAKE_HASH='fade2839ace1a2953556693e6f3d8f9b8b2897894b5a1f2ad477cdf8e9af042a'
PREMAKE_LICENSE='https://github.com/premake/premake-core/blob/master/LICENSE.txt'

InstallPremake ()
{
    # Check requirements
    if [[ ! -x "$(command -v wget)" ]]; then
        echo "Error: wget is required!" >&2
        exit 2
    fi

    if [[ ! -x "$(command -v tar)" ]]; then
        echo "Error: tar is required!" >&2
        exit 2
    fi

    if [[ ! -x "$(command -v sha256sum)" ]]; then
        echo "Error: sha256sum is required!" >&2
        exit 2
    fi

    # Get Premake
    cd external/premake/bin
    wget -nd -O premake.tar.gz "$PREMAKE_URL"
    wget -nd -O LICENSE.txt "$PREMAKE_LICENSE"

    if [[ $? -ne 0 ]]; then
        echo "Error: Download failed!" >&2
        exit 2
    fi

    # Extract Premake
    tar -xf premake.tar.gz

    if [[ $? -ne 0 ]]; then
        echo "Error: Extraction failed!"
        exit 2
    fi

    # Cleanup/Checks
    rm premake.tar.gz
    echo "${PREMAKE_HASH} premake5" | sha256sum -c

    if [[ $? -ne 0 ]]; then
        echo "Error: Hash verification failed!"
        rm premake5
        exit 2
    fi

    chmod +x premake5

    # Change to repository root
    cd "$(dirname "$0")" || exit 2
}

# Change to repository root
cd "$(dirname "$0")" || exit 2

if [[ ! -d ".git" ]]; then
    echo "You must clone the repository using 'git clone'!" >&2
    exit 1
fi

# Check if premake is installed
if [[ ! -x "$(command -v external/premake/bin/premake5)" ]]; then
    echo "Installing premake"
    InstallPremake
fi

git submodule update --init --recursive
external/premake/bin/premake5 gmake
external/premake/bin/premake5 --config=Debug ecc
