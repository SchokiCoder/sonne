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
		begin = ++cursor;
		while (*cursor != '\n' || *cursor != '\0') {
			cursor++;
		}
		cursor--;
		read_len = (cursor - begin);

		t->type = TT_comment;
		t->c.comment = malloc(read_len);
		if (t->c.comment == NULL) {
			*err = TE_malloc_failed;
			return cursor;
		}
		strncpy(t->c.comment, begin, read_len);
		return cursor;
		break;
	
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
		t->c.identifier = malloc(read_len);
		if (t->c.comment == NULL) {
			*err = TE_malloc_failed;
			return cursor;
		}
		strncpy(t->c.identifier, begin, read_len);
		return cursor;
	}

	*err = TE_unrecognized_token;
	return cursor;
}

enum TokenizerError
Tokens_from_file(
	FILE           *f,
	struct Token ***t,
	int             rows,
	int             cols)
{
	char                *cursor;
	enum TokenizerError  err;
	int                  row;
	int                  col;
	char                 line[FILE_LINE_SIZE];
	int                  file_done = 0;
	int                  row_done = 0;

	for (row = 1; !file_done; row++) {
		if (row - 1 > rows) {
			return TE_tbuf_too_small;
		}

		errno = 0;
		line[0] = '\0';
		fgets(line, FILE_LINE_SIZE, f);
		if (feof(f)) {
			file_done = 1;
		}
		if (errno != 0) {
			return TE_file_read_failed;
		}

		err = TE_ok;
		cursor = line;
		row_done = 0;
		for (col = 0; !row_done; col++) {
			if (col > cols) {
				return TE_tbuf_too_small;
			}

			cursor = Token_from_str(t[row][col], cursor, &err, row, col);
			if (err) {
				return err;
			}
			
			if (t[row][col]->type == TT_separator &&
			    t[row][col]->c.separator == '\n') {
				row_done = 1;
			}
		}
	}

	return TE_ok;
}
