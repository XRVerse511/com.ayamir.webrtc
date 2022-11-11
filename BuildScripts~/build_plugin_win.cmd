@echo off

set LIBWEBRTC_PATH=%cd%\..\..\webrtc-checkout\artifacts\webrtc.zip
set SOLUTION_DIR=%cd%\Plugin~

copy %LIBWEBRTC_PATH% .
7z x -aoa webrtc.zip -o%SOLUTION_DIR%\webrtc

echo -------------------
echo Build com.unity.webrtc Plugin

cd %SOLUTION_DIR%
cmake --preset=x64-windows-clang
cmake --build --preset=release-windows-clang --target=WebRTCPlugin