# How to Create and Use Structures (Crews)

W-lang allows you to group related data and functions together using structures, known in the language's slang as a `crew`. This is similar to classes or structs in other languages, but with a simplified, environment-based execution model.

This guide will show you how to define a `crew`, instantiate it, and use its fields and methods.

## 1. Define a Crew

You define a structure using the `crew` keyword. Inside the block, you can declare its fields using the `packing` keyword, and its methods using the `hustle` keyword.

```wlang
summon gossip...

crew Goon {
    psst: Declare fields
    packing tag, muscle...

    psst: Declare a method
    hustle flex() {
        psst: Methods can access fields directly without any 'this' keyword!
        pump_it muscle...
        gossip.spill_tea(tag, " is flexing! Muscle is now: ", muscle)...
    }
}
```

## 2. Recruit (Instantiate) a Crew

To create an instance of your structure, use the `recruit` keyword followed by the structure's name. You can initialize the fields in the exact same order they were defined using the `packing` keyword.

```wlang
gig macho() {
    psst: Instantiate the Goon crew
    stash bouncer about recruit Goon packing "The Wall", 60...
    
    psst: You don't have to initialize all fields. Missing ones become 'ghosted'.
    stash weakling about recruit Goon packing "Slippy"... 
}
```

## 3. Access Fields and Hustles (Methods)

Once you have recruited your instance, you can access its fields and call its methods using the dot (`.`) operator.

```wlang
gig macho() {
    stash bouncer about recruit Goon packing "The Wall", 60...

    psst: Access and modify fields
    gossip.spill_tea("Look out, it's ", bouncer.tag)...
    bouncer.muscle might_be 100...

    psst: Call methods
    bouncer.flex()...
}
```

### Context Binding
W-lang automatically binds the context for you. When `flex()` is called, it inherently knows that `tag` and `muscle` belong to the `bouncer` instance. You never have to write `this`/`self`!