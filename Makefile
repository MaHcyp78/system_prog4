CC := /usr/bin/gcc
BDIR = ./.build/
SDIR = ./src/
FILES = server client

all: create_dir $(FILES)

$(FILES):
	@$(CC) -o $(BDIR)$@ $(SDIR)$(@:%=%.c)

create_dir:
	@./create_dir.sh

clean:
	@rm -rf .build
