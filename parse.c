// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include "parse.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_ParseStatus(const int line, const enum ParseStatus ps)
{
	switch (ps) {
	case PS_ok:
		break;

	case PS_unexpected_line_start:
		printf("Line %i: Unexpected line start\n", line);
	}
}

void text_to_lines(char *text, const int text_len, char **lines, int *n_lines)
{
	int i;

	lines[0] = &text[0];
	*n_lines = 1;

	for (i = 0; i < text_len; i++) {
		if (text[i] == '\n') {
			lines[*n_lines] = &text[i + 1];
			*n_lines += 1;
			text[i] = '\0';
		}
	}
}

char *read_number(char *line, struct Value *no)
{
	char *begin;
	char *end;

	no->type = VT_int;

	begin = line;
	for (; *line != '\0' || (*line >= '0' && *line <= '9'); line++) {}
	end = line;

	errno = 0;
	no->content.i = strtol(begin, &end, 10);
	if (!errno) {
		fprintf(stderr, "Could not read number\n");
	}

	return line;
}

enum ParseStatus parse_line(char *line, struct Scope *scope)
{
	struct Instruction instr;
	struct Value val;

	for (; *line != '\0'; line++) {
		if (*line >= '0' && *line <= '9') {
			line = read_number(line, &val);
			instr.type = IT_express;
			Instruction_add_Value(&instr, val);
			Scope_add_Instruction(scope, instr);
		} else if ((*line >= 'A' && *line <= 'Z') ||
		           (*line >= 'a' && *line <= 'z')) {
			//line = read_symbol(line);
		} else {
			return PS_unexpected_line_start;
		}
	}

	return PS_ok;
}

struct Scope text_to_scope(char *text, const int text_len)
{
	int               i;
	char             *lines[FILE_MAX_LINES];
	int               n_lines = 0;
	enum ParseStatus  ps;
	struct Scope      ret;

	text_to_lines(text, text_len, lines, &n_lines);

	strncpy(ret.name, "main", SCOPE_NAME_MAX_LEN);
	ret.parent = NULL;
	ret.n_instrs = 0;
	ret.n_vars = 0;

	for (i = 0; i < n_lines; i++) {
		ps = parse_line(lines[i], &ret);
		if (ps != PS_ok) {
			print_ParseStatus(i + 1, ps);
		}

		/*
		functions ala pick your own adventure, all of which keep iterating on the char* of that line

		parse_line -> what_is_first_thing - var > look for = -> etc
		                                  - val > look for + -> etc
		*/
	}

	return ret;
}
