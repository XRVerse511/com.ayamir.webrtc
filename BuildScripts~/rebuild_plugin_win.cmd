@echo off

set SOLUTION_DIR=%cd%\Plugin~

echo -------------------
echo Build com.unity.webrtc Plugin

cd %SOLUTION_DIR%
cmake --preset=x64-windows-clang
cmake --build --preset=release-windows-clang --target=WebRTCPlugin