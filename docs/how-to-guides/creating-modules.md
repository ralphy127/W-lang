# How to Create and Use Modules

Modules in W-lang are a feature for organizing your code into reusable and separate files. Instead of writing complex C++ extensions, you can simply create modules directly in W-lang. A module is just another `.weird` file whose functions and global variables can be imported into your main script.

This guide will show you how to create and use a simple module.

## 1. Create the Module File

First, create a new file for your module. Let's call it `my_module.weird`. This file will contain the functions and variables you want to share.

For example, let's create a module with a variable and a function:

```wlang
// my_module.weird

summon gossip...

stash author about "ralphy"...

gig greet(name) {
    gossip.spill_tea("Hello, ", name, "! From ", author)...
}
```

This module defines a global variable `author` and a function `greet` that uses it.

## 2. Import the Module

Now, in your main script (e.g., `main.weird`), you can import `my_module.weird` using the `summon` keyword. The interpreter will look for the file relative to the location of the file that contains the `summon` statement.

```wlang
// main.weird

summon my_module...
```

The `summon` keyword effectively executes the `my_module.weird` file and bundles all of its global definitions (functions and variables) into a single module object named `my_module`.

## 3. Use the Module's Members

Once the module is imported, you can access its functions and variables from within your main `macho` function using the dot (`.`) operator.

```wlang
// main.weird

summon my_module...
summon gossip...

gig macho() {
    // Call the 'greet' function from the module
    my_module.greet("W-lang user")...

    // You can also access global variables from the module
    gossip.spill_tea("The module author is: ", my_module.author)...
}
```

### Running the Code

When you run your main script, the interpreter will find the `macho` function and execute it, handling the module loading automatically.

```bash
./wlang main.weird
```

**Output:**
```
Hello, W-lang user! From ralphy
The module author is: ralphy
```

This approach makes it easy to structure larger projects by breaking them down into smaller, more manageable files.
