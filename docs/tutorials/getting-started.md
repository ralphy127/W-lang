# Getting Started

This tutorial will guide you through writing your first "Hello, World!" program in W-lang.

## 1. Create a File

Create a new file named `hello.weird`.

## 2. Write the Code

W-lang uses the `gossip` module for standard I/O. To print "Hello, World!", you first need to `summon` (import) the `gossip` module. Then, you can call the `spill_tea` function.

```wlang
summon gossip...

gig macho() {
    gossip.spill_tea("Hello, World!")...
}
```

- **`summon gossip...`**: Imports the `gossip` module.
- **`gig macho() { ... }`**: Defines the main function, which is the entry point of the program.
- **`gossip.spill_tea("Hello, World!")...`**: Calls the `spill_tea` function from the `gossip` module to print the string to the console.

## 3. Run the Program

You can run your program using the `wlang` interpreter:

```bash
./build/wlang hello.weird
```

You should see the following output:

```
Hello, World!
```
