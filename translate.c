// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "translate.h"

void
TranslateStatus_print(
	const enum TranslateStatus ts,
	const char *filename,
	const int line,
	const int col)
{
	switch (ts) {
	case TS_ok:
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
