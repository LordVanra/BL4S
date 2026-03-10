@echo off

rmdir /s /q build

if "%G4INSTALL%"=="" if "%GEANT4_DIR%"=="" (
    echo ERROR: GEANT4 environment not found!
    echo Please run your GEANT4 installation setup script first:
    echo   call "C:\path\to\geant4-install\bin\geant4.bat"
    echo.
    pause
    exit /b 1
)

echo GEANT4 installation found.

if not exist build mkdir build
cd build

cmake -G "Visual Studio 17 2022" ..

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    exit /b 1
)

echo Building project...
cmake --build . --config Release

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    exit /b 1
)

echo Build successful!

cd Release
copy ..\vis.mac .
copy ..\run.mac .

protonTransmission.exe