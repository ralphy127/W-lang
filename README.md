# W Language
W is a custom interpreted programming language. It is designed to be a simple language for various applications.

## Main features

- **Hand-written Lexer and Parser**: A custom-built lexer and a recursive descent parser provide full control over the language syntax.
- **Tree-walk Interpreter**: The Abstract Syntax Tree (AST) is traversed by a tree-walk interpreter to execute the code.
- **Advanced Error Handling**: A robust, dual-system approach to error management. User-facing errors (e.g., syntax mistakes) are collected by the lexer and parser into `LexerCrash` and `ParserCrash` objects, allowing the system to report multiple issues in a single pass.

## Documentation

For more detailed information about the language, its architecture, and how to use it, please refer to the most important documentation:

- [Getting Started](docs/tutorials/getting-started.md)
- [Language Guide](docs/reference/language-guide.md)
- [Architecture Overview](docs/explanations/overview.md)

the whole documentation is in the docs directory.

## How to Build

To build the W-lang interpreter, you'll need a C++23 compatible compiler.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/ralphy127/W-lang
    cd W-lang
    ```

2.  **Create a build directory:**
    ```bash
    cd build
    ```

3.  **Configure with CMake:**
    ```bash
    cmake ..
    ```

4.  **Build the project:**
    ```bash
    make
    ```

This will create the `wlang` executable in the `build` directory.

## Usage

To execute a W-lang script, pass the file path to the `wlang` executable:

```bash
./wlang path/to/your/script.weird
```

For example, to run the tic-tac-toe example included in the repository:

```bash
./wlang ../examples/tic_tac_toe.weird
```

## Running Tests

The project uses CTest and GoogleTest for unit and integration testing. To run the test suite, navigate to the `build` directory and run `ctest`:

```bash
cd build
ctest
```
