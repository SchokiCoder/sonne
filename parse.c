// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include "parse.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_def.h"

void
print_ParseStatus(
	const int line,
	const enum ParseStatus ps)
{
	switch (ps) {
	case PS_ok:
		break;

	case PS_unexpected_line_start:
		printf("Line %i: Unexpected line start\n", line);
		break;

	case PS_invalid_number:
		printf("Line %i: Invalid number\n", line);
		break;

	case PS_invalid_operator:
		printf("Line %i: Invalid operator\n", line);
		break;
	}
}

void
text_to_lines(
	char *text,
	const int text_len,
	char **lines,
	int *n_lines)
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

char
*read_number(
	char *line,
	struct Value *no,
	enum ParseStatus *ps)
{
	char *begin;
	char tmp;

	no->type = VT_int;

	begin = line;
	while (*line != '\0' && *line != ' ')
		line++;

	tmp = *line;
	*line = '\0';

	errno = 0;
	no->content.i = strtol(begin, NULL, 10);
	if (errno) {
		*ps = PS_invalid_number;
	}

	*line = tmp;

	return line;
}

char
*read_whitespace(
	char *line)
{
	while (*line == ' ') {
		line++;
	}
	return line;
}

enum ParseStatus
parse_line(
	char *line,
	struct Scope *scope)
{
	enum ParseStatus ps;

	line = read_whitespace(line);

	if (*line >= '0' && *line <= '9') {
		line = parse_math(scope, line, &ps);
		return ps;
	} else if ((*line >= 'A' && *line <= 'Z') ||
	           (*line >= 'a' && *line <= 'z')) {
		// TODO line = read_symbol(line);
		printf("vars not yet implemented (%s)\n", line);
	} else {
		return PS_unexpected_line_start;
	}

	return PS_ok;
}

char
*parse_math(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps)
{
	struct Instruction instr;
	struct Value       val;

	line = read_number(line, &val, ps);
	if (*ps)
		return line;
	Scope_add_tmpval(scope, val);

	instr.type = IT_express;
	line = read_whitespace(line);

	switch (*line) {
	case '\0':
		break;
	case '+':
		instr.type = IT_add;
		break;
	case '-':
		instr.type = IT_sub;
		break;
	case '*':
		instr.type = IT_mul;
		break;
	case '/':
		instr.type = IT_div;
		break;
	default:
		*ps = PS_invalid_operator;
		break;
	}
	line++;

	if (*ps)
		return line;

	if (instr.type == IT_express) {
		Instruction_add_value(&instr, &scope->tmpvals[scope->n_tmpvals -1]);
		Scope_add_instruction(scope, instr);
	} else {
		line = read_whitespace(line);
		line = read_number(line, &val, ps);
		if (*ps) {
			return line;
		}
		Scope_add_tmpval(scope, val);

		val.type = VT_int;
		val.content.i = 0;
		Scope_add_tmpval(scope, val);

		Instruction_add_value(&instr, &scope->tmpvals[scope->n_tmpvals -3]);
		Instruction_add_value(&instr, &scope->tmpvals[scope->n_tmpvals -2]);
		Instruction_add_value(&instr, &scope->tmpvals[scope->n_tmpvals -1]);
		Scope_add_instruction(scope, instr);
	}

	return line;
}

struct Scope
text_to_scope(
	char *text,
	const int text_len)
{
	int               i;
	char             *lines[FILE_MAX_LINES];
	int               n_lines = 0;
	enum ParseStatus  ps;
	struct Scope      ret;

	ret = Scope_new("main", NULL);

	text_to_lines(text, text_len, lines, &n_lines);

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
