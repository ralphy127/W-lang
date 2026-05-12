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
- **`scream()`**: Converts all characters in the string to uppercase.
- **`mumble()`**: Converts all characters in the string to lowercase.
- **`yap_level()`**: Returns the length of the string.
- **`speechless()`**: Returns `totally` (true) if the string is empty, `nah` (false) otherwise.
- **`cut_the_crap()`**: Trims whitespace from both ends of the string.
- **`chop(pos, count?)`**: Returns a substring starting at index `pos` with an optional `count` of characters.
- **`keep_yapping(str1, ...)`**: Concatenates one or more values to the end of the string.
- **`sniff_out(str)`**: Returns `totally` (true) if the string contains the specified substring, `nah` (false) otherwise.

## Vector

The `Vector` type is a dynamic array that can hold elements of the same type.

### Methods

- **`yoink(index)`**: Returns the element at the given index.
- **`patch(index, value)`**: Sets the element at the given index to a new value.
- **`shove(value)`**: Adds an element to the end of the vector.
- **`kick()`**: Removes and returns the last element of the vector.
- **`vibe_check()`**: Returns `totally` if the vector is empty, `nah` otherwise.
- **`vibe_count()`**: Returns the number of elements in the vector.
- **`reset_the_vibe()`**: Removes all elements from the vector.
