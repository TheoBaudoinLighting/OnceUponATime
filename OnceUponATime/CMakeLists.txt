cmake_minimum_required(VERSION 3.30)
project(OnceUponATime)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option for building tests
option(BUILD_TESTS "Build tests" ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/examples)

# Compiler source files (excluding main.cpp for the library)
set(COMPILER_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/code_generator.cpp
    src/ast.cpp
)

# Create the main executable
add_executable(OnceUponATime src/main.cpp ${COMPILER_SOURCES})
target_include_directories(OnceUponATime PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Test configuration with Google Test
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # For Windows: prevent overriding the compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    
    # Create a static library for compiler sources
    add_library(compiler_lib STATIC ${COMPILER_SOURCES})
    target_include_directories(compiler_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
    
    # List of test files located in the tests/ folder
    set(TEST_SOURCES
        tests/lexer_tests.cpp
        tests/parser_tests.cpp
        tests/code_generator_tests.cpp
        tests/ast_tests.cpp
        tests/compiler_tests.cpp
        tests/main_tests.cpp
        tests/integration_tests.cpp
    )
    
    # Create the test executable
    add_executable(OnceUponATime_tests ${TEST_SOURCES})
    target_link_libraries(OnceUponATime_tests PRIVATE compiler_lib gtest gtest_main)
    target_include_directories(OnceUponATime_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
    
    include(GoogleTest)
    gtest_discover_tests(OnceUponATime_tests)
endif()
