@echo off

set LIBWEBRTC_PATH=%cd%\..\webrtc-checkout\artifacts\webrtc.zip
set SOLUTION_DIR=%cd%\Plugin~

copy %LIBWEBRTC_PATH% .
7z x -aoa webrtc.zip -o%SOLUTION_DIR%\webrtc

echo -------------------
echo Build com.unity.webrtc Plugin

cd %SOLUTION_DIR%
cmake . -G "Visual Studio 16 2019" -A x64 -B "build64"
cmake --build build64 --config Release --target WebRTCPlugin