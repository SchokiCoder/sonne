// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <stdio.h>

enum Keyword {
	KW_int,
	KW_float
};

enum ValueType {
	VT_int,
	VT_float
};

void
ValueType_fprint(
	enum ValueType vt,
	FILE *file);

union ValueContent {
	int   i;
	float f;
};

struct Value {
	enum ValueType     type;
	union ValueContent c;
};

void
Value_fprint(
	const struct Value *v,
	FILE *f);

enum TokenizerError {
	TE_ok,
	TE_file_read_failed,
	TE_tbuf_too_small,
	TE_malloc_failed,
	TE_int_read_failed,
	TE_unrecognized_token
};

enum TokenType {
	TT_comment,
	TT_identifier,
	TT_keyword,
	TT_separator,
	TT_operator,
	TT_literal,
	TT_whitespace
};

union TokenC {
	char                 *comment;
	char                 *identifier;
	enum Keyword          keyword;
	char                  separator;
	char                  operator;
	struct Value          literal;
};

struct Token {
	enum TokenType type;
	union TokenC c;
	int row;
	int col;
};

enum TokenizerError
Tokens_from_file(
	FILE           *f,
	struct Token ***t,
	int             rows,
	int             cols);

#endif /* _TOKENIZER_H */
