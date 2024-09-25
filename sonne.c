// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

int
main(
	int argc,
	char *argv[])
{
	int i;
	//struct Scope mainS;
	char *filename;
	char *filepath = NULL;
	FILE *file;
	char *tmp;

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
		} else {
			filepath = argv[i];
		}
	}

	if (filepath == NULL) {
		fprintf(stderr,
		        "No input file, and interactive mode not yet there\n");
		return 0;
	}

	file = fopen(filepath, "r");
	if (file == NULL) {
		fprintf(stderr,
		        "The given filepath:\n"
		        "\"%s\"\n"
		        "is not valid.\n",
		        filepath);
		return 0;
	}

	filename = filepath;
	while (1) {
		tmp = strstr(filename, "/");
		if (tmp != NULL) {
			if (*(tmp + 1) == '\0') {
				fprintf(stderr,
				        "The given filepath:\n"
				        "\"%s\"\n"
				        "is not a single file.\n",
				        filepath);
				return 0;
			}
			filename = tmp + 1;
		} else {
			break;
		}
	}

	struct Token **t;
	t = malloc(sizeof(struct Token) * 10 * 10);
	Tokens_from_file(file, &t, 10, 10);
	//Scope_from_file(&mainS, file, filename);
	fclose(file);


	// statistics for me
	/*printf("# instructions\n");
	for (i = 0; i < mainS.n_instrs; i++) {
		printf("- ");
		Instruction_fprint(&mainS.instrs[i], stdout);
		printf("\n");
	}

	printf("\n# variables\n");
	for (i = 0; i < mainS.n_vars; i++) {
		printf("- \"%s\": ", mainS.var_names[i]);
		Value_fprint(&mainS.var_vals[i], stdout);
		printf("\n");
	}

	printf("\n# tmpvals\n");
	for (i = 0; i < mainS.n_tmpvals; i++) {
		printf("- ");
		Value_fprint(&mainS.tmpvals[i], stdout);
		printf("\n");
	}

	printf("\n# total\n"
	       "instrs: %i\n"
	       "variables: %i\n"
	       "tmpvals: %i\n",
	       mainS.n_instrs,
	       mainS.n_vars,
	       mainS.n_tmpvals);*/


	return 0;
}
