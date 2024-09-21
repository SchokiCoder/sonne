// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include "lang_def.h"

void Instruction_add_Value(struct Instruction *i, struct Value *v)
{
	i->vals[i->n_vals] = v;
	i->n_vals++;
}

void Scope_add_Instruction(struct Scope *s, struct Instruction i)
{
	s->instrs[s->n_instrs] = i;
	s->n_instrs++;
}
