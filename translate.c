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

	case TS_unexpected_line_start:
		printf("%s:%i:%i: Unexpected line start\n", filename, line, col);
		break;

	case TS_unknown_variable_referenced:
		printf("%s:%i:%i: Unknown variable referenced\n",
		       filename, line, col);
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
	int i = 0;

	if (tlen == 0)
		return 0;

	while (t[i].type == TT_whitespace) {
		i++;
	}

	switch (t[i].type) {
	case TT_comment:
		i = skip_tokens_to_statement_end(&t[i], tlen - i);
		return i;
		break;

	case TT_identifier:
		break;

	case TT_keyword:
		break;

	case TT_separator:
		if (t[i].c.separator != '\n') {
			*ts = TS_unexpected_line_start;
			i = skip_tokens_to_statement_end(&t[i], tlen - i);
		}
		return i;
		break;

	case TT_operator:
	case TT_literal:
	case TT_whitespace:
		*ts = TS_unexpected_line_start;
		i = skip_tokens_to_statement_end(&t[i], tlen - i);
		return i;
		break;
	}

	return i;
}
