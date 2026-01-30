# HLIM (HLL Improved)
Impure functional programming language with a simple parser, built upon my
older iteration of the language (HLL).

## Features
For people who are new to the language, it features:
- First-class functions.
- Lexical scoping.
- Function call syntax for everything.

Compared to the old HLL language, HLIM provides:
- Infix notation for arithmetic operations.
- Expression grouping via square brackets.
- Array indexing via `:`, with `@` to index without evaluation on the way.
- The arrow operator `->` (pipeline) to chain function calls easier.
- Lexical scoping (old HLL uses dynamic scope).
- Named parameters for functions/closures.
- Parse time optimization request with `optimize()`.
- Load a C library with `import()`.

## Installation
### Requirements
HLIM was built and tested with these:
- gcc 15.2.1
- make 4.4.1
- bison 3.8.2
- flex 2.6.4
- libffi 3.5.2

### Building
```
make
```

### Installing
```
sudo cp hlim $PREFIX/bin/
```

Replace `$PREFIX` with something like `/usr/` or `/usr/local/`.
`sudo` isn't needed if `$PREFIX` is `~/.local/`.

## Future plans
- Testing on Windows.
- Events.

