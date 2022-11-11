@echo off

set COMMAND_DIR=%~dp0
set ARTIFACTS_DIR=%cd%\artifacts
set OUTPUT_DIR=out
set CPPFLAGS=/WX-
set WEBRTC_ZIP=webrtc.zip
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_MSVS_VERSION=2019
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set vs2019_install=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community

if not exist "%ARTIFACTS_DIR%\lib" (
  mkdir "%ARTIFACTS_DIR%\lib"
)

if not exist %OUTPUT_DIR% (
  mkdir "%OUTPUT_DIR%"
)

setlocal enabledelayedexpansion

for %%i in (x64) do (
  mkdir "%ARTIFACTS_DIR%/lib/%%i"
  for %%j in (false true) do (

    rem generate ninja for release
    call gn gen %OUTPUT_DIR% --root="src" ^
      --args="is_debug=%%j target_cpu=\"%%i\" is_clang=true rtc_include_tests=false rtc_build_examples=false symbol_level=0 enable_iterator_debugging=false proprietary_codecs=true rtc_use_h264=true use_custom_libcxx=false use_custom_libcxx_for_host=false"

    rem build
    ninja -C %OUTPUT_DIR%

    set filename=
    if true==%%j (
      set filename=webrtcd.lib
    ) else (
      set filename=webrtc.lib
    )

    rem copy static library for release build
    copy "%OUTPUT_DIR%\obj\webrtc.lib" "%ARTIFACTS_DIR%\lib\%%i\!filename!"
  )
)

endlocal


rem copy header
xcopy src\*.h "%ARTIFACTS_DIR%\include" /C /S /I /F /H

rem create zip
cd %ARTIFACTS_DIR%
7z a -tzip %WEBRTC_ZIP% *

@REM rem upload to server
@REM scp %WEBRTC_ZIP% ayamir@10.112.79.143:/home/ayamir/