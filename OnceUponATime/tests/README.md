# Tests for OnceUponATime

This directory contains unit tests for the OnceUponATime compiler, implemented with Google Test.

## Test Structure

The tests are organized into several main categories:

1. **Lexer Tests** (`lexer_tests.cpp`): Tests for the lexical analyzer that converts source text into tokens.
2. **Parser Tests** (`parser_tests.cpp`): Tests for the syntax analyzer that converts tokens into sentence structures.
3. **Code Generator Tests** (`code_generator_tests.cpp`): Tests for the code generator that converts sentence structures into target code.
4. **AST Tests** (`ast_tests.cpp`): Tests for the abstract syntax tree.
5. **Compiler Tests** (`compiler_tests.cpp`): Integration tests for the complete compilation process.
6. **Integration Tests** (`integration_tests.cpp`): Tests that verify compilation and execution of generated code.

## How to Run the Tests

### Prerequisites

- CMake (version 3.10 or higher)
- A C++ compiler compatible with C++17
- Git (to download Google Test)

### Building and Running

1. Configure the project with CMake
2. Build the tests
3. Run the generated tests

## Adding New Tests

To add new tests:

1. Identify the appropriate category (lexer, parser, code_generator, etc.).
2. Add new test cases in the corresponding file using Google Test macros (`TEST`).
3. Ensure that new tests are independent of existing tests.
4. Recompile and run the tests to verify they work correctly.

## Tips for Writing Tests

- Each test should be independent and not rely on the state of other tests.
- Use descriptive names for tests to easily understand what they're testing.
- Use appropriate Google Test assertions (`EXPECT_*` for non-fatal checks, `ASSERT_*` for fatal checks).
- Test edge cases and error cases in addition to normal cases.
- Comment your tests to explain what they test and why.

## Troubleshooting

If you encounter problems when running tests:

1. Make sure Google Test has been properly downloaded and configured (this should be automatic with FetchContent).
2. Check that you have the necessary permissions to run the tests.
3. Review the compilation logs for errors.
4. If a test fails, examine the error message to understand why.