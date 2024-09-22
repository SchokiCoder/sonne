# v0.3

- [ ] add functions
which upon finding the '{' in a `symbol() {`,
calls a scope_from_text(), which must end when finding a '}'

# v0.2

- [ ] add y coord to parse error print

- [ ] add using filenames via args to interpret files

- [ ] add actual execution

- [ ] add cli interactive mode

# very bare basics v0.1

- [x] add tmp vals to scope

- [x] fix reading numbers

- [x] add parsing basic math

- [x] add parsing errors for invalid numbers and operators

- [x] fix read_number not catching invalid numbers

- [x] add parse_symbol

- [x] add modulus operator to parse_math

- [ ] make instruction struct opaque
and add functions for constructing all instruction types

- [ ] add "-a" and "-v" flags

- [ ] set version to v0.1
once it fully parses the example:
```
25
400 + 20
i = 0
i + 20
i = i + 1
```
