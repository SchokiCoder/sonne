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

	case PS_unexptected_symbol_followup:
		printf("Line %i: Expected function call, assignment, math, "
		       "or end of line, "
		       "after a given symbol\n",
		       line);
		break;

	case PS_variable_not_found:
		printf("Line %i: Unknown variable referenced\n", line);
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
	while (*line != '\0' && *line != ' ') {
		if (*line < '0' || *line > '9') {
			*ps = PS_invalid_number;
			return line;
		} else {
			line++;
		}
	}

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
*read_symbol(
	char            *line,
	struct Scope    *scope,
	enum SymbolType *st,
	char            **symbol_end_out,
	int             *symbol_idx,
	int             *symbol_found)
{
	char *symbol = line;
	char *symbol_end;
	char tmp;

	while ((*line >= 'A' && *line <= 'Z') ||
	       (*line >= 'a' && *line <= 'z') ||
	       (*line >= '0' && *line <= '9') ||
	       *line == '_') {
		line++;
	}

	symbol_end = line;

	if (symbol_end_out != NULL)
		*symbol_end_out = symbol_end;

	line = read_whitespace(line);

	if (*line == '(') {
		*st = ST_func;
		// TODO *symbol_found = Scope_find_func(scope, symbol, symbol_idx);
	} else {
		*st = ST_var;
		tmp = *symbol_end;
		*symbol_end = '\0';
		*symbol_found = Scope_find_var(scope, symbol, symbol_idx);
		*symbol_end = tmp;
	}

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
		line = parse_math(scope, line, &ps, &scope->expression);
		return ps;
	} else if ((*line >= 'A' && *line <= 'Z') ||
	           (*line >= 'a' && *line <= 'z')) {
		line = parse_symbol(scope, line, &ps);
		return ps;
	} else {
		return PS_unexpected_line_start;
	}

	return PS_ok;
}

char
*parse_math(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps,
	struct Value *assign_target)
{
	struct Value        *first;
	struct Value        *second;
	struct Instruction  instr;
	enum SymbolType     st;
	int                 symbol_idx;
	int                 symbol_found;
	struct Value        val;

	if (*line >= '0' && *line <= '9') {
		line = read_number(line, &val, ps);
		if (*ps) {
			return line;
		}
		Scope_add_tmpval(scope, val);
		first = &scope->tmpvals[scope->n_tmpvals -1];
	} else {
		line = read_symbol(line,
		                   scope,
		                   &st,
		                   NULL,
		                   &symbol_idx,
		                   &symbol_found);

		switch (st) {
		case ST_var:
			first = &scope->var_vals[symbol_idx];
			break;

		case ST_func:
			printf("functions not supported yet\n");
			return line;
			break;
		}
	}

	instr.type = IT_assign;
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

	if (instr.type == IT_assign) {
		Instruction_add_value(&instr, assign_target);
		Instruction_add_value(&instr, first);
		Scope_add_instruction(scope, instr);
	} else {
		line = read_whitespace(line);
		line = read_number(line, &val, ps);
		if (*ps) {
			return line;
		}
		Scope_add_tmpval(scope, val);
		second = &scope->tmpvals[scope->n_tmpvals -1];

		Instruction_add_value(&instr, assign_target);
		Instruction_add_value(&instr, first);
		Instruction_add_value(&instr, second);
		Scope_add_instruction(scope, instr);
	}

	return line;
}

char
*parse_symbol(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps)
{
	struct Instruction  instr;
	enum SymbolType     st;
	char               *symbol = line;
	char               *symbol_end;
	int                 symbol_found;
	char                tmp;
	int                 symbol_idx;

	line = read_symbol(line,
	                   scope,
	                   &st,
	                   &symbol_end,
	                   &symbol_idx,
	                   &symbol_found);

	line = read_whitespace(line);
	tmp = *line;
	*symbol_end = '\0';

	switch (tmp) {
	case '(':
		printf("functions not yet implemented\n");
		return line;
		break;

	case '=':
		instr.type = IT_assign;

		if (!symbol_found) {
			 symbol_idx = scope->n_vars;
			 Scope_add_var(scope, symbol);
		}

		line++;
		line = read_whitespace(line);
		line = parse_math(scope, line, ps, &scope->var_vals[symbol_idx]);
		break;

	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
		if (!symbol_found) {
			*ps = PS_variable_not_found;
			return line;
		}

		instr.type = IT_assign;
		*line = tmp;
		line = parse_math(scope, symbol, ps, &scope->var_vals[symbol_idx]);
		break;

	case '\0':
		if (!symbol_found) {
			*ps = PS_variable_not_found;
			return line;
		}

		instr.type = IT_assign;
		Instruction_add_value(&instr, &scope->expression);
		Instruction_add_value(&instr, &scope->var_vals[symbol_idx]);
		Scope_add_instruction(scope, instr);
		break;

	default:
		*ps = PS_unexptected_symbol_followup;
		return line;
		break;
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
