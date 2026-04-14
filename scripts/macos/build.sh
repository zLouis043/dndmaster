#!/bin/bash
set -e 
cd "$(dirname "$0")/../.."

echo "=== Building DnDMaster ==="
cmake --build build --config Release -j$(sysctl -n hw.logicalcpu)
echo -e "\n=== Build completata con successo! ==="