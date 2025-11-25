@echo off
cd /d "%~dp0"
http-server . -p 8000 -c-1
