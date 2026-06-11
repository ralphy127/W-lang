# W-lang Language Guide

This document provides a comprehensive reference for the syntax, keywords, and semantics of the W-lang programming language.

## Table of Contents

1.  [Comments](#1-comments)
2.  [Variables](#2-variables)
3.  [Data Types](#3-data-types)
4.  [Operators](#4-operators)
5.  [Control Flow](#5-control-flow)
6.  [Loops](#6-loops)
7.  [Functions](#7-functions)
8.  [Structures](#8-structures-crews)
9.  [Modules](#8-modules)
10.  [Built-in Modules](#9-built-in-modules)

---

## 1. Comments

Comments are used to add explanatory notes to the code and are ignored by the interpreter.

- **Single-line comments**: Start with `psst:` and continue until the end of the line.
    ```wlang
    psst: This is a single-line comment.
    ```

- **Multi-line (block) comments**: Start with `rant_stop` and end with `rant_start`.
    ```wlang
    rant_stop
    This is a
    multi-line comment.
    rant_start

    rant_stop You can write comments like this also rant_start
    ```

---

## 2. Variables

Variables are used to store data.

### Declaration

Variables are declared using the `stash` keyword and initialized with the `about` keyword. Every statement in W-lang must end with `...`

```wlang
psst: Declare a variable 'score' and initialize it with the value 100.
stash score about 100...
```

If a variable is declared without an initial value, it defaults to `ghosted` (null).

```wlang
stash playerName... psst: playerName is now 'ghosted'
```

### Reassignment

To change the value of an existing variable, use the `might_be` keyword.

```wlang
stash level about 1...
level might_be 2... psst: Reassign 'level' to 2.
```

---

## 3. Data Types

W-lang has several built-in data types.

| W-lang Name | Description                               | Example                               |
|-------------|-------------------------------------------|---------------------------------------|
| `ghosted`   | The absence of a value (null).            | `ghosted`                             |
| `yap`       | A sequence of characters (text).          | `"Hello, World!"`                     |
| `solid`     | A 32-bit signed integer.                  | `123`, `-45`                          |
| `vibe`      | A boolean value (`totally` or `nah`).     | `totally`, `nah`                      |
| `change`    | A double-precision floating-point number. | `99.9`, `-0.5`                        |
| `crew`      | An instance of a user-defined structure.  | `stash c about recruit Point...`      |
| `lineup`    | A dynamic array of a single type.         | `[1, 2, 3]`                           |
| `gig`       | A user-defined or native function.        | `gig my_func() {}`                    |
| `hub`       | A collection of related code from another file. | `summon my_module...`                 |

### `lineup` (Vector)

A `lineup` is a dynamic array that can only hold elements of a single type.

```wlang
stash myLineup about [10, 20, 30]...
```

---

## 4. Operators

W-lang uses descriptive words for operators.

### Arithmetic Operators

| Operator | Keyword | Example           | Description    |
|----------|---------|-------------------|----------------|
| `+`      | `with`  | `5 with 3`        | Addition       |
| `-`      | `without`| `5 without 3`     | Subtraction    |
| `*`      | `times` | `5 times 3`       | Multiplication |
| `/`      | `over`  | `5 over 3`        | Division       |

### Comparison Operators

| Operator | Keyword | Example         | Description         |
|----------|---------|-----------------|---------------------|
| `==`     | `looks_like` | `a looks_like b` | Equality            |
| `!=`     | `kinda_sus`  | `a kinda_sus b`  | Inequality          |
| `<`      | `tiny_ish`   | `a tiny_ish b`   | Less than           |
| `>`      | `bigger_ish` | `a bigger_ish b` | Greater than        |

### Logical Operators

| Operator | Keyword | Example           | Description                                  |
|----------|---------|-------------------|----------------------------------------------|
| `&&`     | `also`  | `a also b`        | Logical AND (short-circuiting)               |
| `\|\|`   | `either`| `a either b`      | Logical OR (short-circuiting)                |

---

## 5. Control Flow

Conditional logic is handled with `perhaps` statements.

### `perhaps` / `or_whatever` / `screw_it`

This is the equivalent of an `if`/`else` statement.

```wlang
perhaps (x bigger_ish 10) {
    psst: code to run if x is greater than 10
} screw_it {
    psst: code to run otherwise
}
```

### `perhaps` / `or_whatever` / `screw_it`

This is the equivalent of an `if`/`else if`/`else` chain.

```wlang
perhaps (grade looks_like "A") {
    psst: ...
} or_whatever (grade looks_like "B") {
    psst: ...
} screw_it {
    psst: ...
}
```

---

## 6. Loops

W-lang provides two main looping constructs.

### `do_until_bored`

An infinite loop that can be exited using the `break` keyword.

```wlang
do_until_bored {
    perhaps (some_condition) {
        rage_quit!!!
    }
}
```

### `spin_around`

A loop that executes a block of code a specific number of times.

```wlang
psst: Repeats the block 5 times.
spin_around (5) {
    psst: ...
}
```

---

## 7. Functions

Functions are defined with the `gig` keyword.

### Definition

```wlang
gig add(a, b) {
    yeet a with b...
}
```

### Calling

Function calls must end with `...`.

```wlang
stash result about add(5, 3)...
```

### The `macho` function

The `macho` function is the main entry point for any W-lang program. When a script is executed, the interpreter looks for `gig macho()` and starts execution there.

```wlang
gig macho() {
    psst: Your program starts here.
}
```

### `yeet` statement

The `yeet` keyword is used to exit a function and optionally pass back a value. If no value is returned, the function implicitly returns `ghosted`.

```wlang
gig get_ten() {
    yeet 10...
}
```

---

## 8. Structures

### Defining a Structure
Use the `crew` keyword to define the structure, `packing` for fields, and `hustle` for methods. 

```wlang
crew Player {
    packing name, hp...

    hustle heal(amount) {
        hp might_be hp with amount...
    }
}
```
*Note: Methods can access the structure's fields directly without needing any prefix.*

### Instantiation
Use the `recruit` keyword to create an instance. You can pass initial values using `packing`.

```wlang
stash p1 about recruit Player packing "Bob", 10...
```

### Accessing Members
Use the dot (`.`) notation to access fields and methods.

```wlang
p1.hp might_be 20...
p1.heal(50)...
```

---

## 9. Modules

You can split your code into multiple files called modules.

### Creating a Module

A module is any `.weird` file.

```wlang
psst: my_module.weird
gig helper() {
    return "helping!"...
}
```

### Importing a Module

Use the `summon` keyword to import a module. The interpreter creates a `hub` object with the name of the file.

```wlang
psst: main.weird
summon my_module...

gig macho() {
    my_module.helper()...
}
```

---

## 10. Built-in Modules

W-lang comes with some standard modules.

### `gossip`

The `gossip` module provides functions for I/O.

- `gossip.spill_tea(...)`: Prints values to the console.
- `gossip.eavesdrop()`: Reads a line of input from the console.

```wlang
summon gossip...

gig macho() {
    gossip.spill_tea("What's your name?")...
    stash name about gossip.eavesdrop()...
    gossip.spill_tea("Hello, ", name)...
}
```
