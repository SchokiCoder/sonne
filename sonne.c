// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2024  Andy Frank Schoknecht

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize.h"
#include "SVM.h"

int
main(
	int argc,
	char *argv[])
{
	int i;
	struct Module mainM;
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

	Module_from_file(&mainM, file, filename);
	fclose(file);

	Module_fprint(&mainM, stdout);

	Module_free(&mainM);

	return 0;
}
