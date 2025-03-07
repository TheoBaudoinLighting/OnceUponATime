#!/bin/bash
# Script to build and run OnceUponATime tests on Linux/macOS
# Author: Théo Baudoin-Malnoë
# Date: 2025

echo "==================================================="
echo "   Building and running OnceUponATime tests"
echo "==================================================="

# Check if build directory exists, create it if not
if [ ! -d "../build" ]; then
    echo "Creating build directory..."
    mkdir -p "../build"
fi

# Move to the build directory
cd "../build"

# Configure the project with CMake
echo "Configuring project with CMake..."
cmake .. -DBUILD_TESTS=ON

# Build the project
echo "Building the project..."
cmake --build .

echo
echo "==================================================="
echo "   Running tests"
echo "==================================================="

# Run all tests
echo "Running all tests..."
ctest --output-on-failure

# Return to the original directory
cd "../tests"

echo
echo "==================================================="
echo "   Tests completed"
echo "==================================================="