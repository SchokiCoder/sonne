// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include "lang_def.h"

#include <string.h>

void
Instruction_add_value(
	struct Instruction *i,
	struct Value *v)
{
	i->vals[i->n_vals] = v;
	i->n_vals++;
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
