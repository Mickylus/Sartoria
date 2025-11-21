#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
# Compila sart_web
cc -O2 -o sart_web sart_web.c
echo "sart_web compilato"
