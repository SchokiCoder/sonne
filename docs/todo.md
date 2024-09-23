# 0.3.0

- [ ] add functions
which upon finding the '{' in a `symbol() {`,
calls a scope_from_text(), which must end when finding a '}'

# 0.2.0

- [x] add y coord to parse error print
It was actually the x coord lol

- [x] add filename to parse status print

- [x] add using filenames via args to interpret files
Also improve parse end report statistics 

- [x] fix Scope_from_file being a return val
This caused a variety of corruptions

- [x] fix empty line being an "unexpected line start"

- [ ] expression of unknown variable is not triggered?

- [ ] `i + 20s` is ok?

- [ ] Scope.tmpvals is messy
Maybe add another scope array for constants (magic numbers / values).
All tmpvals should only be relevant for the current line of code,
and should be empty in the next line.
`i = i + ((20 + 4) - (6 / 3))`
Needs a lot of tmpvals, which are irrelevant in the next line of code.

- [ ] add runtime environment

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
