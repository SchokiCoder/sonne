// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "SVM.h"

#include <stdlib.h>
#include <string.h>

int
skip_tokens_to_statement_end(
	struct Token *t,
	int tlen);

int
skip_whitespace_tokens(
	struct Token *t,
	int tlen);

int
translate_expression(
	struct Scope *s,
	struct Value *dest,
	struct Token *t,
	int tlen,
	enum TranslateStatus *ts);

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

int
tokens_to_statement(
	struct Token *t,
	int tlen,
	struct Scope *s,
	enum TranslateStatus *ts)
{
	int begin;
	int i = 0;
	int var_idx;

	if (tlen == 0)
		return 0;

	while (i < tlen &&
	       t[i].type == TT_whitespace) {
		i++;
	}

	begin = i;

	switch (t[i].type) {
	case TT_comment:
		i = skip_tokens_to_statement_end(&t[i], tlen - i);
		return i;
		break;

	case TT_identifier:
		if (i + 1 >= tlen) {
			*ts = TS_expected_operator;
			return i;
		}

		i += skip_whitespace_tokens(&t[i], tlen - i);

		if (i >= tlen ||
		    t[i].type != TT_operator ||
		    t[i].c.operator != '=') {
			*ts = TS_expected_operator;
			return i;
		}
		var_idx = Scope_find_var(s, t[begin].c.identifier);
		if (var_idx == -1) {
			var_idx = Scope_add_var(s, t[begin].c.identifier);
		};
		i++;

		i += skip_whitespace_tokens(&t[i], tlen - i);

		i = translate_expression(s, &s->var_vals[var_idx],
		                        &t[i], tlen - i,
		                        ts);

		if (i >= tlen ||
		    (t[i].type != TT_separator &&
		     t[i].c.separator != '\n')) {
			*ts = TS_expected_end_of_statement;
			return i;
		}
		break;

	case TT_keyword:
		break;

	case TT_separator:
		if (t[i].c.separator != '\n') {
			*ts = TS_expected_identifier;
			i = skip_tokens_to_statement_end(&t[i], tlen - i);
		}
		return i;
		break;

	case TT_operator:
	case TT_literal:
	case TT_whitespace:
		*ts = TS_expected_identifier;
		i = skip_tokens_to_statement_end(&t[i], tlen - i);
		return i;
		break;
	}

	return i;
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

int
Scope_from_tokens(
	struct Token *t,
	int tlen,
	struct Scope *s,
	enum TranslateStatus *ts)
{
	int i;

	for (i = 0; i < tlen;) {
		i += tokens_to_statement(&t[i], tlen - i, s, ts);
		switch (*ts) {
		case TS_ok:
			break;

		case TS_new_scope_found:
		case TS_scope_ended:
			return i;
			break;

		default:
			return i;
			break;
		}
	}

	return i;
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

/* Returns index of newly created variable.
 */
int
Scope_add_var(
	struct Scope *s,
	char *name)
{
	s->var_names[s->n_vars] = name;
	s->n_vars++;
	return s->n_vars - 1;
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
		.tc = 0,
		.ssize = 0,
		.slen = 0,
	};
	return ret;
}

int
Module_from_file(
	struct Module        *mod,
	FILE                 *f,
	char                 *filename,
	enum TokenizerError  *te,
	enum TranslateStatus *ts)
{
	int tokens_read;
	int loop;

	*mod = Module_new(filename);
	mod->tsize = 64;
	mod->t = malloc(sizeof(struct Token) * mod->tsize);
	if (mod->t == NULL) {
		return 1;
	}

	for (loop = 1; loop; ) {
		tokens_read = Tokens_from_file(f,
		                               &mod->t[mod->tlen],
		                               mod->tsize - mod->tlen,
		                               te);
		mod->tlen += tokens_read;

		switch (*te) {
		case TE_tbuf_too_small:
			mod->tsize *= 2;
			mod->t = realloc(mod->t, mod->tsize);
			if (mod->t == NULL) {
				return 1;
			}
			break;

		default:
			loop = 0;
			break;
		}
	}

	mod->ssize = 8;
	mod->s = malloc(sizeof(struct Scope) * mod->ssize);
	if (NULL == mod->s) {
		return 1;
	}
	mod->s[mod->slen] = Scope_new(filename, NULL);
	mod->tc = Scope_from_tokens(mod->t, mod->tlen, &mod->s[mod->slen], ts);
	mod->slen++;

	for (loop = 1; loop; ) {
		switch (*ts) {
		case TS_new_scope_found:
			if (mod->slen >= mod->ssize) {
				mod->s = realloc(mod->s,
				                 sizeof(struct Scope) *
				                 mod->ssize * 2);
				if (NULL == mod->s) {
					return 1;
				}
			}

			mod->s[mod->slen] = Scope_new(mod->t[mod->tc].c.identifier,
			                              &mod->s[mod->slen - 1]);
			mod->tc = Scope_from_tokens(&mod->t[mod->tc],
			                            mod->tlen - mod->tc,
			                            &mod->s[mod->slen],
			                            ts);
			mod->slen++;
			break;

		case TS_scope_ended:
			if (&mod->s[0] == mod->s[mod->slen - 1].parent) {
				loop = 0;
				break;
			}

			mod->tc = Scope_from_tokens(&mod->t[mod->tc],
			                            mod->tlen - mod->tc,
			                            mod->s[mod->slen - 1].parent,
			                            ts);
			break;

		default:
			return 0;
			break;
		}
	}

	return 0;
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
	           "ssize = %i\n"
	           "slen = %i\n"
	           "global = ",
	        mod->name,
	        (void*) mod->t,
	        mod->tsize,
	        mod->tlen,
	        mod->ssize,
	        mod->slen);

	if (NULL == mod->s) {
		fprintf(f, "NULL\n");
	} else {
		Scope_fprint(&mod->s[0], f);
	}
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
	mod->tsize = 0;
	mod->tlen = 0;

	free(mod->s);
	mod->ssize = 0;
	mod->slen = 0;
}

int
skip_tokens_to_statement_end(
	struct Token *t,
	int tlen)
{
	int i = 0;

	while (i < tlen &&
	       (t[i].type != TT_separator && t[i].c.separator != '\n')) {
		i++;
	}

	return i + 1;
}

int
skip_whitespace_tokens(
	struct Token *t,
	int tlen)
{
	int i = 0;

	while (i < tlen &&
	       t[i].type != TT_whitespace) {
		i++;
	}

	return i + 1;
}

int
translate_expression(
	struct Scope *s,
	struct Value *dest,
	struct Token *t,
	int tlen,
	enum TranslateStatus *ts)
{
	int a;
	int i = 0;
	struct Instruction instr;
	struct Value *first;
	struct Value *second;
	struct Value  tmpval = {
		.type = VT_int,
		.c.i = 0
	};
	char operator;

	i = skip_whitespace_tokens(&t[i], tlen - i);

	switch (t[i].type) {
	case TT_literal:
		first = &t[i].c.literal;
		break;
	default:
		*ts = TS_expected_value;
		return i;
		break;
	}
	i++;

	i = skip_whitespace_tokens(&t[i], tlen - i);

	if (i >= tlen) {
		*ts = TS_expected_value;
		return i;
	}
	if (t[i].type != TT_operator) {
		*ts = TS_expected_operator;
		return i;
	}
	operator = t[i].c.operator;
	i++;

	i = skip_whitespace_tokens(&t[i], tlen - i);

	if (i >= tlen) {
		*ts = TS_expected_value;
		return i;
	}
	switch (t[i].type) {
	case TT_literal:
		second = &t[i].c.literal;
		break;

	case TT_separator:
		if (t[i].c.separator == '(') {
			second = Scope_add_tmp_val(s, tmpval);
			i = translate_expression(s, second, &t[i], tlen - i, ts);
			if (*ts) {
				return i;
			}
		}
		break;

	default:
		*ts = TS_expected_value;
		return i;
		break;
	}
	i++;

	switch (operator) {
	case '*':
		instr = Instruction_new_mul(dest, first, second);
		break;
	case '/':
		instr = Instruction_new_div(dest, first, second);
		break;
	case '%':
		instr = Instruction_new_modulus(dest, first, second);
		break;

	case '+':
		instr = Instruction_new_add(dest, first, second);
		goto low_prio_instr;
	case '-':
		instr = Instruction_new_sub(dest, first, second);
		goto low_prio_instr;

low_prio_instr:
		for (a = i + 1; a < tlen; a++) {
			switch (t[a].type) {
			case TT_operator:
				if (t[a].c.operator != '+' &&
				    t[a].c.operator != '-') {
					second = Scope_add_tmp_val(s, tmpval);
					i++;
					i = translate_expression(s, second, &t[i], tlen - i, ts);
					if (*ts) {
						return i;
					}
				}
				break;
			case TT_comment:
			case TT_separator:
				a = tlen;
				break;
			default:
				break;
			}
		}
		break;

	default:
		fprintf(stderr, "You are a wizard, Harry.\n");
		return i;
		break;
	}

	Scope_add_instruction(s, instr);
	return i;
}

void
TranslateStatus_print(
	const enum TranslateStatus ts,
	const char *filename,
	const int line,
	const int col)
{
	switch (ts) {
	case TS_ok:
	case TS_new_scope_found:
	case TS_scope_ended:
		break;

	case TS_unknown_variable_referenced:
		printf("%s:%i:%i: Unknown variable referenced\n",
		       filename, line, col);
		break;

	case TS_expected_identifier:
		printf("%s:%i:%i: Expected identifier\n", filename, line, col);
		break;

	case TS_expected_expression:
		printf("%s:%i:%i: Expected value, variable, or function call, "
		       "after mathematical operator or assignment\n",
		       filename, line, col);
		break;

	case TS_expected_operator:
		printf("%s:%i:%i: Expected operator\n",
		       filename, line, col);
		break;

case TS_expected_value:
		printf("%s:%i:%i: Expected value\n", filename, line, col);
		break;

	case TS_expected_end_of_statement:
		printf("%s:%i:%i: Expected end of statement\n",
		       filename, line, col);
		break;
	}
}
