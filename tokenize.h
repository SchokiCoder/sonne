// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#ifndef _TOKENIZE_H
#define _TOKENIZE_H

#include <stdio.h>

enum Keyword {
	KW_int,
	KW_float
};

enum ValueType {
	VT_int,
	VT_float
};

union ValueContent {
	int   i;
	float f;
};

struct Value {
	enum ValueType     type;
	union ValueContent c;
};

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

void
ValueType_fprint(
	enum ValueType vt,
	FILE *file);

void
Value_fprint(
	const struct Value *v,
	FILE *f);

void
Token_fprint(
	const struct Token *t,
	FILE *f);

void
Token_free(
	struct Token *t);

/* f:      file
 * t:      token array
 * buflen: amount elemens that can be held by t
 * err:    pointer to error, if function runs as expected writes ok value here
 * Returns amount of read tokens.
 */
int
Tokens_from_file(
	FILE                *f,
	struct Token        *t,
	int                  buflen,
	enum TokenizerError *err);

#endif /* _TOKENIZE_H */
