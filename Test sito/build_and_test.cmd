@echo off
rem Compila e lancia il test selftest su Windows
cd /d %~dp0






exit /b %errorlevel%Sartoria.exe --selftest)  exit /b 1  echo Compilazione fallitanrem Usa gcc presente in PATH
ngcc -g Sartoria.c -o Sartoria.exe
nif errorlevel 1 (