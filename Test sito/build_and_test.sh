#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
# Compila con gcc
gcc -g Sartoria.c -o Sartoria
# Esegui il selftest
./Sartoria --selftest
echo "Selftest exit: $?"
