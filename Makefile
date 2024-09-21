CC     := cc
CFLAGS := -g -Wall -Wextra -fsanitize=address,undefined

.PHONY: clean

sonne: sonne.c parse.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f sonne
