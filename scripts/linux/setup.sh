#!/bin/bash
set -e 
cd "$(dirname "$0")/../.."

echo "=== Bootstrapping vcpkg ==="
./vendor/vcpkg/bootstrap-vcpkg.sh -disableMetrics

echo -e "\n=== Configuring CMake ==="
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vendor/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
echo -e "\n=== Setup Completato con Successo! ==="