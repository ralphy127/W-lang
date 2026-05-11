# Lexer (Tokenizer)

The lexer reads the raw source code and converts it into a stream of tokens. Each token represents a single lexical unit, such as a keyword, identifier, operator, or literal value. This process, also known as lexical analysis, simplifies the job of the parser by abstracting away whitespace and comments.

Key responsibilities:
- Skipping whitespace and comments.
- Recognizing keywords, identifiers, and literals.
- Accumulating syntax errors.