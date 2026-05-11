# Logging

W-lang provides a simple logging mechanism to help with debugging the interpreter itself.

## Log Levels

There are four log levels defined in `src/utils/Logging.hpp`.

- `Debug`
- `Info`
- `Warn`
- `Error`

## Command-Line Flag (`--debug`)

As an alternative to manually editing the source code, you can enable debug logging by passing the `--debug` flag when running the interpreter from the command line. This is the recommended way to get more detailed logs for a single run without having to recompile.

```bash
./wlang /path/to/your/script.weird --debug
```

Now, when you run the interpreter, it will produce detailed debug logs.

## Log File Location

All logs are written to the `build/logs.txt` file. This file is created or overwritten each time you run the interpreter.
