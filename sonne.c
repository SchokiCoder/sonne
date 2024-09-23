// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang_def.h"
#include "parse.h"

#define SCRIPT "25\n"\
               "400 + 20\n"\
               "i = 0\n"\
               "i + 20\n"\
               "i = i + 1"

int
main(
	int argc,
	char *argv[])
{
	int i;
	struct Scope mainS;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			printf("%s: version %s\n", APP_NAME, APP_VERSION);
			return 0;
		} else if (strcmp(argv[i], "-a") == 0) {
			printf("The source code of \"%s\" v%s is available, "
			       "licensed under the %s at:\n"
			       "%s\n\n"
			       "If you did not receive a copy of the license, "
			       "see below:\n"
			       "%s\n",
			       APP_NAME, APP_VERSION,
			       APP_LICENSE,
			       APP_REPO,
			       APP_LICENSE_URL);
			return 0;
		}
	}


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
	printf("# instructions\n");
	for (i = 0; i < mainS.n_instrs; i++)
		printf("- type: %i, vals: %i\n",
			mainS.instrs[i].type, mainS.instrs[i].n_vals);

	printf("\n# variables\n");
	for (i = 0; i < mainS.n_vars; i++)
		printf("- name: \"%s\", type: %i, int: %i\n",
			mainS.var_names[i], mainS.var_vals[i].type, mainS.var_vals[i].content.i);

	printf("\n# tmpvals\n");
	for (i = 0; i < mainS.n_tmpvals; i++)
		printf("- type: %i, int: %i\n",
			mainS.tmpvals[i].type, mainS.tmpvals[i].content.i);

	printf("\n# total\ninstrs: %i\ntmpvals: %i\n",
		mainS.n_instrs, mainS.n_tmpvals);


	return 0;
}
