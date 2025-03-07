@echo off
REM Script to build and run OnceUponATime tests on Windows
REM Author: Théo Baudoin-Malnoë
REM Date: 2025

setlocal enabledelayedexpansion

echo ===================================================
echo    Building and running OnceUponATime tests
echo ===================================================

REM Check if build directory exists, create it if not
if not exist "..\build" (
    echo Creating build directory...
    mkdir "..\build"
)

REM Move to the build directory
cd "..\build"

REM Configure the project with CMake
echo Configuring project with CMake...
cmake .. -DBUILD_TESTS=ON

REM Build the project
echo Building the project...
cmake --build . --config Debug

echo.
echo ===================================================
echo    Running tests
echo ===================================================

REM Run all tests
echo Running all tests...
ctest -C Debug --output-on-failure

REM Return to the original directory
cd "..\tests"

echo.
echo ===================================================
echo    Tests completed
echo ===================================================

endlocal 