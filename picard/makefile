LOGIN1 = ls30687
LOGIN2 = ls31285
GRUP = 18
FASE = 3
CFLAGS = -Wall -Wextra
MAIN = main.c
MODULES = logica utils network network_utils shell
EXE = picard

FILES = $(MAIN) $(patsubst %,%.c, $(MODULES))
HEADERS = $(patsubst %,%.h, $(MODULES))

all: $(FILES) $(HEADERS)
	gcc $(FILES) $(CFLAGS) -o $(EXE)

stop:
	@ps -u $(LOGIN1) | grep -e $(EXE) -e memcheck-amd64- | awk '{print $$1}' | xargs kill 2> /dev/null
	@echo "Service stopped."

again: clean all

clean:
	rm -f $(EXE)

tar:
	tar -cf G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar $(FILES) $(HEADERS) makefile
