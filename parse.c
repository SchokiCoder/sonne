// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include "parse.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_def.h"

void
ParseStatus_print(
	const enum ParseStatus ps,
	const char *filename,
	const int line,
	const int col)
{
	switch (ps) {
	case PS_ok:
		break;

	case PS_unexpected_line_start:
		printf("%s:%i:%i: Unexpected line start\n", filename, line, col);
		break;

	case PS_invalid_number:
		printf("%s:%i:%i: Invalid number\n", filename, line, col);
		break;

	case PS_invalid_operator:
		printf("%s:%i:%i: Invalid operator\n", filename, line, col);
		break;

	case PS_unexptected_symbol_followup:
		printf("%s:%i:%i: Expected function, or assignment, "
		       "after an initial symbol\n",
		       filename, line, col);
		break;

	case PS_variable_not_found:
		printf("%s:%i:%i: Unknown variable referenced\n",
		       filename, line, col);
		break;

	case PS_unexpected_operator_followup:
		printf("%s:%i:%i: Expected value, variable, or function call, "
		       "after mathematical operator\n",
		       filename, line, col);
		break;

	case PS_expected_operator:
		printf("%s:%i:%i: Expected operator\n",
		       filename, line, col);
		break;
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
	while (*line != '\0' && *line != ' ' && *line != '\n') {
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

char
*parse_line(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps)
{
	line = read_whitespace(line);

	if (*line == '\n' || *line == '\0') {
		return line;
	} else if ((*line >= 'A' && *line <= 'Z') ||
	           (*line >= 'a' && *line <= 'z')) {
		line = parse_linestart_symbol(scope, line, ps);
		return line;
	} else {
		*ps = PS_unexpected_line_start;
		return line;
	}

	*ps = PS_ok;
	return line;
}

char
*parse_math(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps,
	struct Value *dest)
{
	struct Instruction  instr;
	struct Value       *m_left;
	struct Value       *m_right;
	struct Value        val;
	struct Word         word[2];

	line = Word_from_line(&word[0], line, scope, ps);
	if (*ps) {
		return line;
	}

	switch (word[0].type) {
	case WT_operator:
		m_left = dest;
		goto operator_is_read;
		break;

	case WT_variable:
		m_left = word[0].c.var;
		break;

	case WT_value:
		m_left = Scope_add_tmpval(scope, word[0].c.val);
		break;
	}

	line = Word_from_line(&word[0], line, scope, ps);
	if (*ps) {
		return line;
	}

	if (word[0].type != WT_operator) {
		*ps = PS_expected_operator;
		return line;
	}

operator_is_read:

	line = read_whitespace(line);
	if (*line == '(') {
		m_right = Scope_add_tmpval(scope, val);
		line = parse_math(scope, line, ps, m_right);
	} else {
		Word_from_line(&word[1], line, scope, ps);
		if (*ps) {
			return line;
		}
		switch (word[1].type) {
		case WT_operator:
			*ps = PS_unexpected_operator_followup;
			return line;
			break;

		case WT_variable:
			m_right = word[1].c.var;
			break;

		case WT_value:
			m_right = Scope_add_tmpval(scope, word[1].c.val);
			break;
		}
	}

	instr = Instruction_new_math(word[0].c.op, dest, m_left, m_right);
	Scope_add_instruction(scope, instr);

	return line;
}

char
*parse_linestart_symbol(
	struct Scope *scope,
	char *line,
	enum ParseStatus *ps)
{
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
		if (!symbol_found) {
			 symbol_idx = scope->n_vars;
			 Scope_add_var(scope, symbol);
		}

		line++;

		line = parse_math(scope, line, ps, &scope->var_vals[symbol_idx]);
		break;

	default:
		*ps = PS_unexptected_symbol_followup;
		return line;
		break;
	}

	return line;
}
