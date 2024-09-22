// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _PARSE_H
#define _PARSE_H

#include "lang_def.h"

#define FILE_MAX_LINES 512

enum ParseStatus {
	PS_ok,
	PS_unexpected_line_start,
	PS_invalid_number,
	PS_invalid_operator,
	PS_unexptected_symbol_followup,
	PS_variable_not_found
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
	struct Value *no,
	enum ParseStatus *ps);

char
*read_operator(
	char *line,
	enum InstructionType *it,
	enum ParseStatus *ps);

char
*read_whitespace(
	char *line);

enum ParseStatus
parse_line(
	char *line,
	struct Scope *scope);

char
*parse_math(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps,
	struct Value *assign_target);

char
*parse_symbol(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps);

struct
Scope text_to_scope(
	char *text,
	const int text_len);

#endif /* _PARSE_H */
