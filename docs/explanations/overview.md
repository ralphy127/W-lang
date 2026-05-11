# Architecture Overview

The W-lang interpreter follows a traditional pipeline, transforming source code into executable actions. This document explains the major components and data flow.

## The Pipeline

The compilation process is a linear sequence of steps, where the output of one stage becomes the input for the next:

1.  **Source Code**: The raw text written by the user in a `.weird` file.
    
    `↓`

2.  **Lexer (Tokenizer)**: Reads the source code and breaks it down into a series of tokens (e.g., keywords, identifiers, numbers, operators).
    
    `↓`

3.  **Parser**: Takes the stream of tokens and organizes them into a hierarchical structure called an Abstract Syntax Tree (AST), based on the language's grammar.
    
    `↓`

4.  **Interpreter (Tree-Walk Evaluator)**: Traverses the AST, executing the logic defined in each node. This is where the code's behavior is realized.
    
    `↓`

5.  **Output / Side Effects**: The final result of the program, such as printing to the console, modifying variables, or returning a value.

This modular design allows each component to be developed and tested independently. The main components are:

- **Lexer**: Converts source code into a stream of tokens.
- **Parser**: Builds an Abstract Syntax Tree (AST) from the tokens.
- **Interpreter**: Executes the code by traversing the AST.
