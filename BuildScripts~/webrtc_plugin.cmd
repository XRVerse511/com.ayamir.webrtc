@echo off

set SOLUTION_DIR=%cd%\Plugin~

echo -------------------
echo Download LibWebRTC

scp ayamir@10.112.79.143:/home/ayamir/webrtc.zip webrtc.zip
7z x -aoa webrtc.zip -o%SOLUTION_DIR%\webrtc

echo -------------------
echo Build com.ayamir.webrtc Plugin

cd %SOLUTION_DIR%
cmake . -G "Visual Studio 16 2019" -A x64 -B "build64"
cmake --build build64 --config Release --target WebRTCPlugin