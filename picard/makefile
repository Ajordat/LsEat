EXE = picard
LOGIN = ls30687
CC = gcc
CFLAGS = -Wall -Wextra
COP = -o
FILES = main.c logica.c utils.c
HEADERS = logica.h utils.h
TEMP_FILES = *.o

all: $(FILES) $(HEADERS)
	$(CC) $(FILES) $(CFLAGS) $(COP) $(EXE)

stop:
	ps -u $(LOGIN) | grep $(EXE) | awk '{print $$1}' | xargs kill 2> /dev/null

again: clean all

clean:
	rm -f $(EXE) $(TEMP_FILES)
