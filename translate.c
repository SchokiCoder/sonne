// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "translate.h"

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

int
skip_tokens_to_statement_end(
	struct Token *t,
	int tlen)
{
	int i = 0;

	while (i < tlen ||
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

	while (i < tlen ||
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

int
tokens_to_scope(
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
