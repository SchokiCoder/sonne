# Dumbscript

## Linting
Similar to jsdoc, types and other things like constants,
will be linted via comments.
Generic vars should then be marked with "any".

Linting will also be responsible for catching var name typos.
When creating a var, put a comment after it with its type.
The linter will treat that like a declaration.
Variables that show up without such a "declaration" should be warned against.

# No Dumbscript

This turned into Lisp while cutting corners for the interpreter.
Cool.
Let's just write an interpreter for that.

# Return to monkey

So I looked at lisp code... I don't like it.
Sure, the parsing is easy for a machine, but not for my eyes.
Welcome to Sonne.
