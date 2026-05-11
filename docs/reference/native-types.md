# Native Types

W-lang comes with a set of built-in native types that provide fundamental data structures. These types are represented by the `RuntimeValueType` enum in the C++ source.

## Available Types

- **`Null`**: Represents the absence of a value. The keyword `ghosted` is used for this type.
- **`String`**: A sequence of characters.
- **`Int`**: A 32-bit signed integer.
- **`Bool`**: A boolean value, can be `totally` (true) or `nah` (false).
- **`Float`**: A double-precision floating-point number.
- **`Vector`**: A dynamic array that can hold elements of the same type.
- **`Function`**: A user-defined or native function.
- **`Module`**: A collection of related functions and variables, imported with the `summon` keyword.

## String

The `String` type represents a sequence of characters.

### Methods

- **`to_solid()`**: Converts a string to an integer, if possible.

## Vector

The `Vector` type is a dynamic array that can hold elements of the same type.

### Methods

- **`get(index)`**: Returns the element at the given 1-based index.
- **`set(index, value)`**: Sets the element at the given 1-based index to a new value.
- **`push_back(value)`**: Adds an element to the end of the vector.
- **`pop_back()`**: Removes and returns the last element of the vector.
- **`is_empty()`**: Returns `totally` if the vector is empty, `nah` otherwise.
- **`size()`**: Returns the number of elements in the vector.
- **`clear()`**: Removes all elements from the vector.
