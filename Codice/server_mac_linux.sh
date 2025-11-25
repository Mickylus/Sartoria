#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

if command -v http-server >/dev/null 2>&1; then
  http-server . -p 8000 -c-1
elif command -v npx >/dev/null 2>&1; then
  npx http-server . -p 8000 -c-1
elif command -v python3 >/dev/null 2>&1; then
  echo "http-server non trovato, uso python3 -m http.server (senza controllo cache)"
  python3 -m http.server 8000
else
  echo "Installa node + http-server (npm i -g http-server) o python3" >&2
  exit 1
fi