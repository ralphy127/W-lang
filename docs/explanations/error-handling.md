# Error Handling in W-lang

W-lang employs a two-pronged approach to error handling to ensure both user-friendliness and developer insight. This document outlines the mechanisms for handling user-facing errors (lexical, syntactical and runtime) and internal errors.

## User-Facing Errors: `LexerCrash` and `ParserCrash`

When a user writes code that violates the language's rules, the program must report these errors in a clear and helpful way. W-lang achieves this by collecting all errors during the lexing and parsing phases before halting compilation.

### The Process

1.  **Lexical Analysis**: The `Lexer` tokenizes the source code. If it encounters invalid sequences of characters (e.g., an unterminated string), it doesn't stop immediately. Instead, it records a `LexerError` and attempts to continue tokenizing the rest of the file.
2.  **Error Aggregation**: All `LexerError` instances are collected into a `LexerResult`. If any errors were found, this result is wrapped in a `LexerCrash` exception and thrown.
3.  **Parsing**: If lexing succeeds, the `Parser` receives the stream of tokens. As it builds the Abstract Syntax Tree (AST), it validates the token sequence against the language grammar. Any syntactic violations (e.g., a missing parenthesis) are recorded as `ParserError` objects.
4.  **Synchronization and Recovery**: After detecting a syntax error, the parser enters a "synchronization" mode. It discards tokens until it finds a point where it can safely resume parsing (e.g., the beginning of a new statement). This allows it to detect multiple, independent errors in a single pass.
5.  **Error Reporting**: If any `ParserError`s were recorded, they are collected and thrown within a `ParserCrash` exception.

The `main` function of the interpreter contains `try...catch` blocks that catch `LexerCrash` and `ParserCrash`. The `ErrorReporter` is then used to iterate through the stored errors and print them to the console in a formatted, easy-to-read way, pointing to the exact location of each error in the source file and providing context window.

This "store and report" strategy significantly improves the user experience, as developers can see all syntax and lexical errors at once instead of having to fix them one by one.

## User-Facing Runtime Errors

Errors that occur during the execution of the program are caught by the interpreter. Unlike lexical and parsing errors, the interpreter throws immediately when a runtime error is encountered, as the program state may be inconsistent. These errors are primarily handled through two mechanisms: `RuntimeError` and `NativeError`.

-   **`RuntimeError`**: This struct is used for errors that can be directly traced back to a specific portion of the user's source code. It contains the error type, a descriptive message, and a `SourceRange` that pinpoints the exact location of the error in the code. This allows the `ErrorReporter` to show the user precisely which line is causing the issue.

-   **`NativeError`**: This is an exception thrown by the native C++ code that implements W's built-in functions (e.g., functions within modules). When the interpreter calls one of these native functions and it fails, it catches the `NativeError`, wraps its message and type into a `RuntimeError` (attaching the source location of the function call in the user's script), and then reports it. This bridges the gap between the C++ backend and the W-lang script, providing meaningful error messages to the user.

## Internal Errors

Internal errors represent bugs within the interpreter itself—situations that should never occur during the compilation of valid or invalid user code. For these, W-lang uses a "fail-fast" approach.

When an unexpected state is detected, an `InternalError` is thrown. This is a custom exception that captures a `std::source_location` at the point of failure. A top-level `try...catch` block in `main` catches this error, prints the error message and the source location (file, line, and function name), and immediately terminates the interpreter.

This provides developers of the W-lang with a detailed, actionable report to debug and fix internal logic flaws.

## Error Examples

Here are some examples of how W-lang reports different types of errors.

### Lexer Errors

Lexer errors occur when the source code contains invalid characters or sequences that cannot be turned into tokens.

**Example 1: Unterminated String**

```
../tests/test_files/lexer_errors.weird:5:20 - Yap has no end quote!
     4 |        stash ok about 123...
     5 |        gossip.spill_tea("this string is never closed...
       |                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     6 |        @ # [ ] it $ % &[]
```

**Example 2: Unknown Character**

```
../tests/test_files/lexer_errors.weird:6:4 - What the heck is this character?
     5 |        gossip.spill_tea("this string is never closed...
     6 |        @ # [ ] it $ % &[]
       |          ^
     7 | }
```

### Parser Errors

Parser errors happen when the sequence of tokens is syntactically incorrect, meaning it violates the language's grammar rules.

**Example 1: Missing Function Name**

```
../tests/test_files/parser_errors.weird:3:5 - Bruh, missing name right after gig
     2 | 
     3 | gig () {
       |     ^
     4 |        yeet 0...
```

**Example 2: Missing Block Opening**

```
../tests/test_files/parser_errors.weird:14:2 - Bruh, missing {, need it to open a code snippet for do_until_bored
    13 | do_until_bored
    14 |        yeet 2...
       |        ^
    15 |
```

### Interpreter (Runtime) Errors

Runtime errors occur during program execution.

**Example 1: Type Mismatch in Math Operation**

```
../tests/test_files/runtime_errors/01_math_mismatch.weird:2:10 - [MathOops] Math is only mathing on numbers
     1 | gig macho() {
     2 |     yeet 1 with "2"...
       |          ^^^^^^^
     3 | }
```

**Example 2: Mixed Types in Vector**

```
../tests/test_files/runtime_errors/04_vector_mixed_types.weird:2:19 - [Nope] Lineup got mixed vibes - all elements must be the same type
     1 | gig macho() {
     2 |     stash v about [1, "two"]...
       |                   ^^^^^^^^^
     3 | }
```