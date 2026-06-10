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

Here is the rest of the documentation:

- [Error Handling](docs/explanations/error-handling.md)
- [Keywords](docs/reference/keywords.md)
- [Native Types](docs/reference/native-types.md)
- [Type System](docs/explanations/type-system.md)
- [Creating Structures](docs/how-to-guides/creating-structs.md)
- [Creating Modules](docs/how-to-guides/creating-modules.md)
- [Logging](docs/explanations/logging.md)
- [Interpreter](docs/explanations/interpreter.md)
- [Parser](docs/explanations/parser.md)
- [Lexer](docs/explanations/lexer.md)

## Supported Platforms

W-lang is officially supported and continuously tested on the following platforms:
- **macOS** (latest) with Apple Clang
- **Linux** (Ubuntu latest) with GCC-14

## Installation

### Option 1: Using Pre-built Binaries (Release)

You can download a pre-built binary for your operating system directly from the [GitHub Releases](https://github.com/ralphy127/W-lang/releases) page.

1. Download the `.tar.gz` file for your platform (macOS or Linux).
2. Extract the archive:
   ```bash
   tar -xzf w-lang-*.tar.gz
   ```
3. (Optional) Move the binary to a directory included in your `$PATH` (e.g., `~/.local/bin`) so you can run it from anywhere:
   ```bash
   mkdir -p ~/.local/bin
   mv wlang ~/.local/bin/
   ```

### Option 2: Building from source

To build and install the W-lang interpreter from source, you'll need a C++23 compatible compiler.
An easy, interactive installation script is provided.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/ralphy127/W-lang
    cd W-lang
    ```

2.  **Run the install script:**
    ```bash
    ./install.sh
    ```

The script will automatically build the project using CMake, install the `wlang` executable to `$HOME/.local/bin`, and optionally add the directory to your `PATH` for easy execution.

## Usage

Once you have installed the project (and `$HOME/.local/bin` is in your `PATH`), you can execute a script anywhere:

```bash
wlang path/to/your/script.weird
```

If you chose not to add it to your `PATH`, you can use the absolute path:

```bash
~/.local/bin/wlang path/to/your/script.weird
```

## Running Tests

The project uses CTest and GoogleTest for unit and integration testing. To run the test suite, navigate to the `build` directory and run `ctest`:

```bash
cd build
ctest
```

## CI/CD

W-lang uses GitHub Actions for continuous integration and deployment:
- **Automated Testing (`ci.yaml`)**: Every push to `main` and active development branches triggers a build and test run across all supported platforms to ensure stability.
- **Automated Releases (`release.yml`)**: Pushing a new version tag (e.g., `v*`) automatically compiles the project in Release mode without any caching overhead and attaches the properly packaged binaries to a new GitHub Release.
