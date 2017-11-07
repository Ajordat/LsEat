LOGIN1 = ls30687
LOGIN2 = ls31285
GRUP = 18
FASE = 2
CC = gcc
CFLAGS = -Wall -Wextra -lpthread
MAIN = main.c
MODULES = logica utils network
FILES = $(patsubst %,%.c, $(MODULES))
HEADERS = $(patsubst %,%.h, $(MODULES))
TEMP_FILES =
EXE = enterprise

all: $(MAIN) $(FILES) $(HEADERS)
	$(CC) $(MAIN) $(FILES) $(CFLAGS) -o $(EXE)

stop:
	ps -u $(LOGIN1) | grep $(EXE) | awk '{print $$1}' | xargs kill 2> /dev/null

again: clean all

clean:
	rm -f $(EXE) $(TEMP_FILES)

tar:
	tar -cf G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar $(FILES) $(HEADERS) makefile
