// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "lang_def.h"

#include <errno.h>
#include <string.h>

#include "parse.h"

void
Instruction_add_value(
	struct Instruction *i,
	struct Value *v);

struct Instruction
Instruction_new_math(
	enum InstructionType type,
	struct Value *dest,
	struct Value *left,
	struct Value *right);

void
Instruction_add_value(
	struct Instruction *i,
	struct Value *v)
{
	i->vals[i->n_vals] = v;
	i->n_vals++;
}

void
InstructionType_fprint(
	enum InstructionType it,
	FILE *file)
{
	switch (it) {
	case IT_mov:
		fprintf(file, "mov");
		break;
	case IT_add:
		fprintf(file, "add");
		break;
	case IT_sub:
		fprintf(file, "sub");
		break;
	case IT_mul:
		fprintf(file, "mul");
		break;
	case IT_div:
		fprintf(file, "div");
		break;
	case IT_modulus:
		fprintf(file, "modulus");
		break;
	}
}

struct Instruction
Instruction_new_math(
	enum InstructionType type,
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	struct Instruction ret;

	ret.type = type;
	Instruction_add_value(&ret, dest);
	Instruction_add_value(&ret, left);
	Instruction_add_value(&ret, right);

	return ret;
}

struct Instruction
Instruction_new_mov(
	struct Value *dest,
	struct Value *src)
{
	struct Instruction ret;

	ret.type = IT_mov;
	Instruction_add_value(&ret, dest);
	Instruction_add_value(&ret, src);

	return ret;
}

struct Instruction
Instruction_new_add(
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	return Instruction_new_math(IT_add, dest, left, right);
}

struct Instruction
Instruction_new_sub(
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	return Instruction_new_math(IT_sub, dest, left, right);
}

struct Instruction
Instruction_new_mul(
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	return Instruction_new_math(IT_mul, dest, left, right);
}

struct Instruction
Instruction_new_div(
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	return Instruction_new_math(IT_div, dest, left, right);
}

struct Instruction
Instruction_new_modulus(
	struct Value *dest,
	struct Value *left,
	struct Value *right)
{
	return Instruction_new_math(IT_modulus, dest, left, right);
}

void
Instruction_fprint(
	const struct Instruction *instr,
	FILE *f)
{
	int i;

	InstructionType_fprint(instr->type, f);
	for (i = 0; i < instr->n_vals; i++) {
		fprintf(f, " %i", instr->vals[i]->content.i);
	}
}

struct Scope
Scope_new(
	char *name,
	struct Scope *parent)
{
	struct Scope ret = {
		.parent = parent,
		.n_instrs = 0,
		.n_vars = 0,
		.n_tmpvals = 0
	};
	strncpy(ret.name, name, SCOPE_NAME_MAX_LEN);
	return ret;
}

struct Scope
Scope_from_file(
	FILE *file,
	char *const filename)
{
	char             *cursor;
	int               i;
	char              line[FILE_LINE_SIZE];
	enum ParseStatus  ps;
	int               reading = 1;
	struct Scope      ret;
	struct Scope     *root;

	ret = Scope_new(filename, NULL);

	for (i = 0; reading; i++) {
		errno = 0;
		fgets(line, FILE_LINE_SIZE, file);
		if (feof(file)) {
			reading = 0;
		}
		if (errno != 0) {
			fprintf(stderr,
			        "There was an error, reading \"%s\":\n"
			        "%i\n",
			        filename,
			        errno);
			reading = 0;
			break;
		}

		ps = PS_ok;
		cursor = line;
		cursor = parse_line(&ret, cursor, &ps);
		if (ps != PS_ok) {
			root = &ret;
			while (root->parent != NULL) {
				root = root->parent;
			}
			print_ParseStatus(ps, root->name, i + 1, cursor - line);
		}
	}

	return ret;
}

void
Scope_add_instruction(
	struct Scope *s,
	struct Instruction i)
{
	s->instrs[s->n_instrs] = i;
	s->n_instrs++;
}

void
Scope_add_var(
	struct Scope *s,
	char *name)
{
	strncpy(s->var_names[s->n_vars], name, VARIABLE_NAME_MAX_LEN);
	s->n_vars++;
}

void
Scope_add_tmpval(
	struct Scope *s,
	struct Value v)
{
	s->tmpvals[s->n_tmpvals] = v;
	s->n_tmpvals++;
}

int
Scope_find_var(
	struct Scope *s,
	char *name,
	int *idx)
{
	int i;

	for (i = 0; i < s->n_vars; i++) {
		if (strcmp(name, s->var_names[i]) == 0) {
			*idx = i;
			return 1;
		}
	}

	return 0;
}
