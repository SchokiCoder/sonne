// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _LANG_DEF_H
#define _LANG_DEF_H

#define SCOPE_MAX_INSTRUCTIONS 2048
#define SCOPE_MAX_VARIABLES 128
#define SCOPE_MAX_TMPVALUES 128
#define SCOPE_NAME_MAX_LEN 128
#define VARIABLE_NAME_MAX_LEN 64

enum ValueType {
	VT_int,
	VT_float
};

union ValueContent {
	int   i;
	float f;
};

struct Value {
	enum ValueType     type;
	union ValueContent content;
};

enum InstructionType {
	IT_express,
	IT_assign,
	IT_add,
	IT_sub,
	IT_mul,
	IT_div,
	IT_modulus
};

struct Instruction {
	enum InstructionType type;
	int                  n_vals;
	struct Value         *vals[8];
};

void
Instruction_add_value(
	struct Instruction *i,
	struct Value *v);

struct Scope {
	char                name[SCOPE_NAME_MAX_LEN];
	struct Scope       *parent;
	int                 n_instrs;
	struct Instruction  instrs[SCOPE_MAX_INSTRUCTIONS];
	int                 n_vars;
	char                var_names[SCOPE_MAX_VARIABLES][VARIABLE_NAME_MAX_LEN];
	struct Value        var_vals[SCOPE_MAX_VARIABLES];
	int                 n_tmpvals;
	struct Value        tmpvals[SCOPE_MAX_TMPVALUES];
};

struct Scope
Scope_new(
	char         *name,
	struct Scope *parent);

void
Scope_add_instruction(
	struct Scope *s,
	struct Instruction i);

void
Scope_add_var(
	struct Scope *s,
	char *name);

void
Scope_add_tmpval(
	struct Scope *s,
	struct Value v);

/* s: Scope
 * name: Name of variable
 * idx: Saving index of variable in that scope to that pointer location
 * Returns non zero if var is found.
 */
int
Scope_find_var(
	struct Scope *s,
	char *name,
	int *idx);

#endif /* _LANG_DEF_H */
