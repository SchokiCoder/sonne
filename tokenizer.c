// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "tokenizer.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define FILE_LINE_SIZE 128 /* if you have more, stop it, get some help */

char
*Token_from_str(
	struct Token        *t,
	char                *cursor,
	enum TokenizerError *err,
	int row,
	int col);

void
ValueType_fprint(
	enum ValueType vt,
	FILE *file)
{
	switch (vt) {
	case VT_int:
		fprintf(file, "int");
		break;
	case VT_float:
		fprintf(file, "float");
		break;
	}
}

void
Value_fprint(
	const struct Value *v,
	FILE *f)
{
	ValueType_fprint(v->type, f);
	switch (v->type) {
	case VT_int:
		fprintf(f, "(%i)", v->c.i);
		break;
	case VT_float:
		fprintf(f, "(%f)", v->c.f);
		break;
	}
}

char
*Token_from_str(
	struct Token        *t,
	char                *cursor,
	enum TokenizerError *err,
	int row,
	int col)
{
	char *begin;
	int read_len;
	char tmp;

	t->row = row;
	t->col = col;

	switch (*cursor) {
	case '#':
		begin = cursor;
		while (*cursor != '\n' && *cursor != '\0') {
			cursor++;
		}
		read_len = (cursor - begin);

		t->type = TT_comment;
		t->c.comment = malloc(read_len + 1);
		if (t->c.comment == NULL) {
			*err = TE_malloc_failed;
			return cursor;
		}
		t->c.comment[read_len] = '\0';
		strncpy(t->c.comment, begin, read_len);
		return cursor;
		break;
	
	case '=':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
		t->type = TT_operator;
		t->c.operator = *cursor;
		cursor++;
		return cursor;
		break;

	case '(':
	case ')':
	case '{':
	case '}':
	case ',':
		t->c.separator = *cursor;
		goto after_separator_assignment;
	case '\n':
	case '\0':
		t->c.separator = '\n';

after_separator_assignment:
		t->type = TT_separator;
		cursor++;
		return cursor;
		break;

	default:
		break;
	}

	begin = cursor;
	while (*cursor == ' ' || *cursor == '\t') {
		cursor++;
	}
	if (cursor > begin) {
		t->type = TT_whitespace;
		return cursor;
	}

	begin = cursor;
	while (*cursor >= '0' && *cursor <= '9') {
		cursor++;
	}
	if (cursor > begin) {
		t->type = TT_literal;

		tmp = *cursor;
		*cursor = '\0';
		t->c.literal.type = VT_int;
		errno = 0;
		t->c.literal.c.i = strtol(begin, NULL, 10);
		if (errno) {
			*err = TE_int_read_failed;
		}
		*cursor = tmp;

		return cursor;
	}

	begin = cursor;
	while ((*cursor >= 'A' && *cursor <= 'Z') ||
	       (*cursor >= 'a' && *cursor <= 'z')) {
		cursor++;
	}
	if (cursor > begin) {
		read_len = (cursor - begin);

		t->type = TT_identifier;
		t->c.identifier = malloc(read_len + 1);
		if (t->c.identifier == NULL) {
			*err = TE_malloc_failed;
			return cursor;
		}
		t->c.identifier[read_len] = '\0';
		strncpy(t->c.identifier, begin, read_len);
		return cursor;
	}

	*err = TE_unrecognized_token;
	return cursor;
}

void
Token_fprint(
	const struct Token *t,
	FILE *f)
{
	switch (t->type) {
	case TT_comment:
		fprintf(f, "TT_comment(%s)", t->c.comment);
		break;

	case TT_identifier:
		fprintf(f, "TT_identifier(%s)", t->c.identifier);
		break;

	case TT_keyword:
		fprintf(f, "TT_keyword(%i)", t->c.keyword);
		break;

	case TT_separator:
		fprintf(f, "TT_separator(%c)", t->c.separator);
		break;

	case TT_operator:
		fprintf(f, "TT_operator(%c)", t->c.operator);
		break;

	case TT_literal:
		fprintf(f, "TT_literal(");
		Value_fprint(&t->c.literal, f);
		fprintf(f, ")");
		break;

	case TT_whitespace:
		fprintf(f, "TT_whitespace(NA)");
		break;
	}
}

void
Token_free(
	struct Token *t)
{
	switch (t->type) {
	case TT_comment:
		free(t->c.comment);
		break;
	case TT_identifier:
		free(t->c.identifier);
		break;
	default:
		break;
	}
}

int
Tokens_from_file(
	FILE                *f,
	struct Token        *t,
	int                  buflen,
	enum TokenizerError *err)
{
	char                *cursor;
	int                  i = 0;
	int                  row;
	int                  col;
	char                 line[FILE_LINE_SIZE];
	int                  file_done = 0;
	int                  row_done = 0;

	for (row = 1; !file_done; row++) {
		errno = 0;
		line[0] = '\0';
		fgets(line, FILE_LINE_SIZE, f);
		if (feof(f)) {
			file_done = 1;
		}
		if (errno != 0) {
			*err = TE_file_read_failed;
			return i;
		}

		*err = TE_ok;
		cursor = line;
		row_done = 0;
		for (; !row_done; i++) {
			if (i >= buflen) {
				*err = TE_tbuf_too_small;
				return i;
			}

			col = cursor - line;
			cursor = Token_from_str(&t[i], cursor, err, row, col);
			if (*err) {
				return i;
			}
			
			if (t[i].type == TT_separator &&
			    t[i].c.separator == '\n') {
				row_done = 1;
			}
		}
	}

	*err = TE_ok;
	return i;
}
