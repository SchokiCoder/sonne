// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _PARSE_H
#define _PARSE_H

#include "lang_def.h"

#define FILE_MAX_LINES 512

enum ParseStatus {
	PS_ok,
	PS_unexpected_line_start
};

void
print_ParseStatus(
	const int line,
	const enum ParseStatus ps);

void
text_to_lines(
	char *text,
	const int text_len,
	char **lines,
	int *n_lines);

char
*read_number(
	char *line,
	struct Value *no);

enum ParseStatus
parse_line(
	char *line,
	struct Scope *scope);

char
*linestart_number(
	struct Scope *scope,
	char *line);

struct
Scope text_to_scope(
	char *text,
	const int text_len);

#endif /* _PARSE_H */
