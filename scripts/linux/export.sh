#!/bin/bash
set -e 
cd "$(dirname "$0")/../.."

echo "=== Creazione Pacchetto di Installazione (CPack) ==="
cd build
cpack -C Release
cd ..

echo -e "\n=== TUTTO PRONTO! ==="
echo "Trovi il file impacchettato (.tar.gz o .deb) dentro la cartella 'build'."