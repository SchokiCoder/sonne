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
	// looks ahead for '*', '/'; make instructions for those, then the rest
	// calls itself upon '(' with dest being a temp_val in scope
	// stops at ')', ',', '\n'
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
