CC := gcc
CFLAGS := -g -Wall

.PHONY: all
all: build

build: src/main.c
	$(CC) $(CFLAGS) src/main.c -o avoidLogout

.PHONY: clean
clean:
	rm -rf *.o avoidLogout
