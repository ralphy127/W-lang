# Parser

The parser takes the stream of tokens from the lexer and constructs an Abstract Syntax Tree (AST). It applies the rules of the language's grammar to the token stream, ensuring the code is syntactically correct. Our parser is a hand-written recursive descent parser, which allows for fine-grained control and clear error reporting.

The W-lang follows a **"Smart Parser, Dumb AST"** philosophy. The parser is responsible for all semantic validation. This means that by the time the AST is constructed, it is guaranteed to be valid. The AST nodes themselves are simple Data Transfer Objects (DTOs) with no logic.

## Error Accumulation and Recovery

A key feature of the W-lang parser is its ability to perform error accumulation. Instead of stopping and exiting at the very first syntax error it encounters, the parser is designed to be resilient.

When a parsing error occurs (e.g., an unexpected token), the parser will:
1.  **Record the error**: It creates a `ParserError` object containing the problematic token and a descriptive message.
2.  **Enter recovery mode**: It attempts to "synchronize" by discarding tokens until it finds a token that is likely to start a new, valid statement (like `gig`, `stash`, `perhaps`, etc.).
3.  **Continue parsing**: Once synchronized, it resumes parsing from that point.

This process allows the parser to analyze the entire source file and report multiple syntax errors in a single compilation attempt, which is much more user-friendly than forcing the user to fix errors one by one. All collected `ParserError` objects are then wrapped in a `ParserCrash` exception and thrown at the end of the parsing phase.

