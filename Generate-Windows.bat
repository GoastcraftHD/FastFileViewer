echo off
set PREMAKE_URL="https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-windows.zip"
set PREMAKE_HASH="C34A6E0B15F119F6284886298FDD8DF543AF87AD16F3CE5F4D0A847BE2A88343"
set PREMAKE_LICENSE="https://github.com/premake/premake-core/blob/master/LICENSE.txt"

goto start

:InstallPremake

if not exist "external/premake/bin" mkdir "external/premake/bin"

powershell -NoProfile -NonInteractive -Command "Invoke-WebRequest %PREMAKE_URL% -OutFile external/premake/bin/premake.zip"
powershell -NoProfile -NonInteractive -Command "Invoke-WebRequest %PREMAKE_URL% -OutFile external/premake/bin/LICENSE.txt"

IF ERRORLEVEL 1 (
    echo "Error: Download failed!"
    exit 2
)

powershell -NoProfile -NonInteractive -Command "Expand-Archive -LiteralPath external/premake/bin/premake.zip -DestinationPath external/premake/bin -Force"
IF ERRORLEVEL 1 (
    echo "Error: Extraction failed!"
    exit 2
)

powershell -NoProfile -NonInteractive -Command "Remove-Item -Path external/premake/bin/premake.zip"

powershell -NoProfile -NonInteractive -Command "if ((Get-FileHash -LiteralPath external/premake/bin/premake5.exe -Algorithm SHA256).Hash -eq \"%PREMAKE_HASH%\") { exit 0 } else { exit 1 }"
IF ERRORLEVEL 1 (
    echo "Error: Hash verification failed!"
    powershell -NoProfile -NonInteractive -Command "Remove-Item -Path external/premake/bin/premake5.exe"
    exit 2
)

exit /B 0

cd %~dp0

:start

IF EXIST "external/premake/bin/premake5.exe" (
    external\premake\bin\premake5.exe --version >NUL
    IF NOT ERRORLEVEL 1 (
        goto runpremake
    )
)

echo "Installing premake"
call:InstallPremake
goto runpremake

:runpremake
git submodule update --init --recursive
external\premake\bin\premake5.exe vs2022
PAUSE
