# 0.3.0

- [ ] add functions
which upon finding the '{' in a `symbol() {`,
calls a scope_from_text(), which must end when finding a '}'

# 0.2.0

- [x] add y coord to parse error print

- [ ] add filename to parse error print

- [ ] add using filenames via args to interpret files

- [ ] add actual execution

- [ ] add cli interactive mode

-----

# very bare basics 0.1.0

- [x] add tmp vals to scope

- [x] fix reading numbers

- [x] add parsing basic math

- [x] add parsing errors for invalid numbers and operators

- [x] fix read_number not catching invalid numbers

- [x] add parse_symbol

- [x] add modulus operator to parse_math

- [x] add functions for constructing all instruction types
Also rename assign to mov.
I don't yet make instruction struct opaque,
because that means they must live in heap,
and I'd have to see how to all of this efficiently.

- [x] add flags for about and version info
Also change license to 2.1-only version of LGPL.

- [x] set version to 0.1.0
once it fully parses the example:
```
25
400 + 20
i = 0
i + 20
i = i + 1
```
