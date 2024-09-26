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

- [x] fix some wonkiness when last line is empty
It partially kept the previous line, thus interpreting it again.

- [x] remove expressions
This will later be a thing that is fully contained within interactive mode,
and otherwise is faulty code,
because why would you toss meaningless statements into actual scripts anyway?

- [x] add comments

- [x] rework towards separate tokenizing

- [ ] add translation based on tokens
Add Module struct.
todo: fill Module scope



- [ ] add per line data `struct Statement`,
which have their own instructions and tmp_vals

- [ ] rework parse_math to actually be fully useful
if first thing we find is operator, `dest` will become `first`, unless dest is NULL, that is a parse error
if dest is empty, we need tmpval for that
if we find a '(', call parse_math
if we indd a ')', return from parse_math

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
