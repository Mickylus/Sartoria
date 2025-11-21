@echo off
cd /d %~dp0





echo Compilazione sart_web completata)  exit /b 1  echo Compilazione sart_web fallitanrem Compila sart_web su Windows usando gcc in PATH
ngcc -O2 -o sart_web.exe sart_web.c
nif errorlevel 1 (