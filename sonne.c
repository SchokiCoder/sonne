// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2024  Andy Frank Schoknecht

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_def.h"
#include "parse.h"

#define SCRIPT "25\n400 + 20\ni = 0\ni = i + 1"

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

	// statistics for me
	int i;

	printf("# instructions\n");
	for (i = 0; i < mainS.n_instrs; i++)
		printf("- type: %i, vals: %i\n",
			mainS.instrs[i].type, mainS.instrs[i].n_vals);

	printf("\n# tmpvals\n");
	for (i = 0; i < mainS.n_tmpvals; i++)
		printf("- type: %i, int: %i\n",
			mainS.tmpvals[i].type, mainS.tmpvals[i].content.i);

	printf("\n# total\ninstrs: %i\ntmpvals: %i\n",
		mainS.n_instrs, mainS.n_tmpvals);


	return 0;
}
