# W-lang Keywords

This document lists all the reserved keywords natively available in W-lang and provides a brief explanation of their purpose.

## Variable Declarations & Assignment

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `stash` | `let` / `var` | Declares a new variable. |
| `about` | `=` | Initial assignment operator for when a variable is instantiated. |
| `might_be` | `=` | Reassignment operator to update an existing variable. |

## Functions & Modules

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `gig` | `func` / `fn` | Declares a user-defined function. |
| `yeet` | `return` | Returns a value from a function. |
| `summon` | `import` | Imports a module or file. |

## Control Flow

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `perhaps` | `if` | Begins a conditional block. |
| `or_whatever` | `else if` | Alternative condition in a block. |
| `screw_it` | `else` | Default fallback condition in a conditionally executed block. |

## Loops

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `do_until_bored` | `while` | Basic loop that acts upon a condition. |
| `spin_around` | `for` | General iteration construct block. |
| `rage_quit` | `break` | Exits a loop early. |

## Boolean & Null Literals

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `totally` | `true` | The truthy boolean literal. |
| `nah` | `false` | The falsy boolean literal. |
| `ghosted` | `null` / `nil` | Represents the absence of a value. |

## Logical Operators

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `also` | `&&` / `and` | Logical AND operator. |
| `either` | `\|\|` / `or` | Logical OR operator. |

## Comparison Operators

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `looks_like` | `==` | Equality comparison operator. |
| `kinda_sus` | `!=` | Inequality comparison operator. |
| `bigger_ish` | `>` | Greater than comparison operator. |
| `tiny_ish` | `<` | Less than comparison operator. |

## Arithmetic & Math Operators

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `with` | `+` | Addition operator. |
| `without` | `-` | Subtraction operator. |
| `times` | `*` | Multiplication operator. |
| `over` | `/` | Division operator. |
| `pump_it` | `++` | Increment operator. |

## Native Types

| Keyword | Description |
| :--- | :--- |
| `vibe` | Boolean (bool). |
| `solid` | Integer (int). |
| `change` | Double-precision floating-point number (double). |
| `yap` | Text string. |
| `lineup` | Vector (dynamic array). |

## Punctuation & Statement Terminators

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `...` | `;` | Standard statement terminator. |
| `!!!` | `;` | Break statement terminator used specifically after `rage_quit`. |

## Others

| Keyword | Traditional Equivalent | Description |
| :--- | :--- | :--- |
| `hustle` | `method` | Represents a method on a complex type (like string or vector). |

## Runtime Error Types

When things go wrong, W-lang uses its own slang for runtime error categories:

| Error | Internal Type | Description |
| :--- | :--- | :--- |
| `Mystery` | `Undefined` | Used for undefined behaviors or variables. |
| `BrainLag` | `Logic` | Represents logic errors in the code execution. |
| `MathOops` | `Math` | Specifically for mathematical errors like division by zero. |
| `OuttaBounds` | `OutOfBounds` | Indicates illegal access to an array or vector index. |
| `Nope` | `TypeMismatch` | Thrown when an operation is performed on incompatible types. |
