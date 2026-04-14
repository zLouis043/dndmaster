#!/bin/bash
set -e 
cd "$(dirname "$0")/../.."

echo "=== Building DnDMaster ==="
cmake --build build --config Release -j$(nproc)
echo -e "\n=== Build completata con successo! ==="