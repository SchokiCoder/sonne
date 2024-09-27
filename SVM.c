// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "SVM.h"

#include <stdlib.h>
#include <string.h>

#include "translate.h"

void
Instruction_add_value(
	struct Instruction *i,
	struct Value *v);

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
		fprintf(f, " ");
		Value_fprint(instr->vals[i], f);
	}
}

struct Scope
Scope_new(
	char *name,
	struct Scope *parent)
{
	struct Scope ret = {
		.name = name,
		.parent = parent,
		.n_literals = 0,
		.n_tmp_vals = 0,
		.n_vars = 0,
		.n_instrs = 0
	};
	return ret;
}

void
Scope_fprint(
	struct Scope *s,
	FILE *f)
{
	fprintf(f, "<---\nScope begin\n"
	           "name = \"%s\"\n"
	           "parent = %p\n"
	           "n_literals = %i\n"
	           "literals = %p\n"
	           "n_tmp_vals = %i\n"
	           "tmp_vals = %p\n"
	           "n_vars = %i\n"
	           "var_names = %p\n"
	           "var_vals = %p\n"
	           "n_instrs = %i\n"
	           "instrs = %p\n"
	           "Scope end\n--->\n",
	        s->name, (void*) s->parent, s->n_literals, (void*) s->literals,
	        s->n_tmp_vals, (void*) s->tmp_vals, s->n_vars,
	        (void*) s->var_names, (void*) s->var_vals, s->n_instrs,
	        (void*) s->instrs);
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
	s->var_names[s->n_vars] = name;
	s->n_vars++;
}

struct Value
*Scope_add_tmp_val(
	struct Scope *s,
	struct Value v)
{
	s->tmp_vals[s->n_tmp_vals] = v;
	s->n_tmp_vals++;
	return &s->tmp_vals[s->n_tmp_vals - 1];
}

int
Scope_find_var(
	struct Scope *s,
	char *name)
{
	int i;

	for (i = 0; i < s->n_vars; i++) {
		if (strcmp(name, s->var_names[i]) == 0) {
			return i;
		}
	}

	return -1;
}

struct Module
Module_new(
	char *name)
{
	struct Module ret = {
		.name = name,
		.tsize = 0,
		.tlen = 0,
		.global = Scope_new(name, NULL)
	};
	return ret;
}

enum TokenizerError
Module_from_file(
	struct Module *mod,	
	FILE *f,
	char *filename)
{
	enum TokenizerError te;
	enum TranslateStatus ts;
	int tokens_read;

	*mod = Module_new(filename);
	mod->tsize = 64;
	mod->t = malloc(sizeof(struct Token) * mod->tsize);
	if (mod->t == NULL) {
		return TE_malloc_failed;
	}

	while (1) {
		tokens_read = Tokens_from_file(f,
		                               &mod->t[mod->tlen],
		                               mod->tsize - mod->tlen,
		                               &te);
		mod->tlen += tokens_read;

		switch (te) {
		case TE_tbuf_too_small:
			mod->tsize *= 2;
			mod->t = realloc(mod->t, mod->tsize);
			if (mod->t == NULL) {
				return TE_malloc_failed;
			}
			break;

		default:
			return te;
			break;
		}
	}

	tokens_to_scope(mod->t, mod->tlen, &mod->global, &ts);
	switch (ts) {
	case TS_new_scope_found:
		make new scope and call on that scope;
		break;

	case TS_scope_ended:
		break;

	default:
		something went wrong;
		break;
	}
}

void
Module_fprint(
	struct Module *mod,
	FILE *f)
{
	fprintf(f, "<---\nModule begin\n"
	           "name = \"%s\"\n"
	           "t = %p\n"
	           "tsize = %i\n"
	           "tlen = %i\n"
	           "global = ",
	        mod->name, (void*) mod->t, mod->tsize, mod->tlen);
	Scope_fprint(&mod->global, f);
	fprintf(f, "Module end\n--->\n");
}

void
Module_free(
	struct Module *mod)
{
	int i;

	for (i = 0; i < mod->tlen; i++) {
		Token_free(&mod->t[i]);
	}
	free(mod->t);
}
