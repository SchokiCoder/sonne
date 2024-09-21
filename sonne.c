// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_def.h"
#include "parse.h"

#define SCRIPT "25\ni = 0\ni = i + 1"

int
main()
{
	struct Scope mainS;


	/* small hack for now, until we read actual source files */
	int script_len = strlen(SCRIPT);
	char *text = malloc(script_len + 1);
	if (text == NULL) {
		fprintf(stderr, "Couldn't malloc for file buffer\n");
		return 0;
	}
	strncpy(text, SCRIPT, script_len);
	text[script_len] = '\0';

	mainS = text_to_scope(text, script_len + 1);


	free(text); // hack cleanup


	return 0;
}
