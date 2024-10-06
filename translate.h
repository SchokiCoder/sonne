// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _TRANSLATE_H
#define _TRANSLATE_H

#include "SVM.h"
#include "tokenize.h"

enum TranslateStatus {
	TS_ok,
	TS_new_scope_found,
	TS_scope_ended,
	TS_unknown_variable_referenced,
	TS_expected_identifier,
	TS_expected_operator,
	TS_expected_expression,
	TS_expected_value,
	TS_expected_end_of_statement,
};

void
TranslateStatus_print(
	const enum TranslateStatus ts,
	const char *filename,
	const int row,
	const int col);

/* Returns amount of translated tokens.
 */
int
tokens_to_scope(
	struct Token *t,
	int tlen,
	struct Scope *s,
	enum TranslateStatus *ts);

/* Returns amount of translated tokens.
 */
int
tokens_to_statement(
	struct Token *t,
	int tlen,
	struct Scope *s,
	enum TranslateStatus *ts);

#endif /* _TRANSLATE_H */
