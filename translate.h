// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _TRANSLATE_H
#define _TRANSLATE_H

#include "SVM.h"
#include "tokenize.h"

enum TranslateStatus {
	TS_ok,
	TS_unexpected_line_start,
	TS_unknown_variable_referenced,
	TS_expected_operator,
	TS_expected_expression
};

void
TranslateStatus_print(
	const enum TranslateStatus ts,
	const char *filename,
	const int row,
	const int col);

#endif /* _TRANSLATE_H */
