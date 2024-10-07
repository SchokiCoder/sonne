# SPDX-License-Identifier: LGPL-2.1-only
# Copyright (C) 2024  Andy Frank Schoknecht

APP_NAME        := \"sonne\"
APP_VERSION     := \"0.1.0\"
APP_LICENSE     := \"LGPL-2.1-only\"
APP_REPO        := \"https://github.com/SchokiCoder/sonne\"
APP_LICENSE_URL := \"https://spdx.org/licenses/LGPL-2.1-only.html\"

CC      := cc
CFLAGS  := -std=c99 -pedantic -g -Wall -Wextra -fsanitize=address,undefined
DEFINES := -D APP_NAME=$(APP_NAME) \
	-D APP_VERSION=$(APP_VERSION) \
	-D APP_LICENSE=$(APP_LICENSE) \
	-D APP_REPO=$(APP_REPO) \
	-D APP_LICENSE_URL=$(APP_LICENSE_URL)

.PHONY: clean

sonne: sonne.c SVM.c tokenize.c
	$(CC) $(CFLAGS) $(DEFINES) $^ -o $@

clean:
	rm -f sonne
