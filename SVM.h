// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _SVM_H
#define _SVM_H

#include <stdio.h>

#include "tokenize.h"

#define SCOPE_MAX_INSTRUCTIONS 2048
#define SCOPE_MAX_LITERALS     128
#define SCOPE_MAX_VARIABLES    128
#define SCOPE_MAX_TMP_VALUES   128

enum InstructionType {
	IT_mov,
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

struct Scope {
	char               *name;
	struct Scope       *parent;
	int                 n_literals;
	struct Value        literals[SCOPE_MAX_LITERALS];
	int                 n_tmp_vals;
	struct Value        tmp_vals[SCOPE_MAX_TMP_VALUES];
	int                 n_vars;
	char               *var_names[SCOPE_MAX_VARIABLES];
	struct Value        var_vals[SCOPE_MAX_VARIABLES];
	int                 n_instrs;
	struct Instruction  instrs[SCOPE_MAX_INSTRUCTIONS];
};

struct Module {
	char         *name;
	struct Token *t;
	int           tsize;
	int           tlen;
	struct Scope  global;
};

void
InstructionType_fprint(
	enum InstructionType it,
	FILE *file);

struct Instruction
Instruction_new_math(
	enum InstructionType type,
	struct Value *dest,
	struct Value *left,
	struct Value *right);

struct Instruction
Instruction_new_mov(
	struct Value *dest,
	struct Value *src);

struct Instruction
Instruction_new_add(
	struct Value *dest,
	struct Value *left,
	struct Value *right);

struct Instruction
Instruction_new_sub(
	struct Value *dest,
	struct Value *left,
	struct Value *right);

struct Instruction
Instruction_new_mul(
	struct Value *dest,
	struct Value *left,
	struct Value *right);

struct Instruction
Instruction_new_div(
	struct Value *dest,
	struct Value *left,
	struct Value *right);

struct Instruction
Instruction_new_modulus(
	struct Value *dest,
	struct Value *left,
	struct Value *right);

void
Instruction_fprint(
	const struct Instruction *i,
	FILE *f);

struct Scope
Scope_new(
	char         *name,
	struct Scope *parent);

void
Scope_fprint(
	struct Scope *s,
	FILE *f);

void
Scope_add_instruction(
	struct Scope *s,
	struct Instruction i);

void
Scope_add_var(
	struct Scope *s,
	char *name);

/* Returns pointer to newly created value.
 */
struct Value
*Scope_add_tmp_val(
	struct Scope *s,
	struct Value v);

/* s:    Scope
 * name: Name of variable
 * idx:  Saving index of variable in that scope to that pointer location
 * Returns index if found, otherwise returns -1
 */
int
Scope_find_var(
	struct Scope *s,
	char *name);

struct Module
Module_new(
	char *name);

enum TokenizerError
Module_from_file(
	struct Module *mod,	
	FILE *f,
	char *filename);

void
Module_fprint(
	struct Module *mod,
	FILE *f);

void
Module_free(
	struct Module *mod);

#endif /* _SVM_H */
