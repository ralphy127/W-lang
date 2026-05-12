# Type System

W-lang uses a dynamic type system, but with some static features enforced at runtime. The core of the type system is the `RuntimeValue` struct, which is a `std::variant` that can hold any of the possible runtime types.

## `RuntimeValue` and `RuntimeValueType`

- **`RuntimeValue`**: A C++ `std::variant` that can hold any of the language's types (`Int`, `Float`, `String`, `Vector`, etc.). This allows for dynamic typing, as a variable can hold different types at different times.
- **`RuntimeValueType`**: An `enum class` that corresponds to each type in the `RuntimeValue` variant. It's used for type checking at runtime.

## Operator Overloading

W-lang supports operator overloading for its native types. The overloads are implemented as free functions in `src/runtime/RuntimeValue.hpp`. This allows for natural-looking expressions like `a with b` for addition.

The supported overloaded operators are:
- `+` (addition)
- `-` (subtraction)
- `*` (multiplication)
- `/` (division)
- `==` (equality)
- `!=` (inequality)
- `<=` (less than or equal to)
- `>=` (greater than or equal to)

Type checking for these operations is performed at runtime. If an operator is used with incompatible types (e.g., adding a `String` to an `Int`), a `RuntimeError` is thrown.

## Available Types

Here is a list of all built-in types available in W-lang, along with their corresponding C++ type and their name in the language's slang:

| W-lang Name | C++ Type (`RuntimeValue`)              | Description                               |
|-------------|----------------------------------------|-------------------------------------------|
| `ghosted`   | `Null` (`std::monostate`)              | Represents the absence of a value (null). |
| `yap`       | `String` (`std::string`)               | A sequence of characters (text).          |
| `solid`     | `Int` (`std::int32_t`)                 | A 32-bit signed integer.                  |
| `vibe`      | `Bool` (`bool`)                        | A boolean value (`true` or `false`).      |
| `change`    | `Float` (`double`)                     | A double-precision floating-point number. |
| `lineup`    | `Vector` `(std::vector<RuntimeValue>)` | A dynamic array that can hold elements.   |
| `gig`       | `Function` `(std::function<RuntimeValue(const std::vector<RuntimeValue>&)>)` | A user-defined or native function.        |
| `hub`       | `Module` `(std::shared_ptr<Environment>)` | A collection of related functions and variables. |
