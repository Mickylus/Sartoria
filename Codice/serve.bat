@echo off
REM Serve the Codice folder on port 8000 using Python's simple server
pushd "%~dp0"
python -m http.server 8000
popd
