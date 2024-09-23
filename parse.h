// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _PARSE_H
#define _PARSE_H

#include "lang_def.h"

enum ParseStatus {
	PS_ok,
	PS_unexpected_line_start,
	PS_invalid_number,
	PS_invalid_operator,
	PS_unexptected_symbol_followup,
	PS_variable_not_found
};

enum SymbolType {
	ST_var,
	ST_func
};

void
print_ParseStatus(
	const enum ParseStatus ps,
	const char *filename,
	const int line,
	const int col);

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
*read_symbol(
	char            *line,
	struct Scope    *scope,
	enum SymbolType *st,
	char            **symbol_end_out,
	int             *symbol_idx,
	int             *symbol_found);

char
*read_whitespace(
	char *line);

char
*parse_line(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps);

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
