LOGIN1 = ls30687
LOGIN2 = ls31285
GRUP = 18
FASE = 3
CFLAGS = -Wall -Wextra -lpthread
MAIN = main.c
MODULS = logica utils network
FILES := $(MAIN) $(patsubst %,%.c,$(MODULS))
HEADERS := $(patsubst %,%.h,$(MODULS))
EXE = enterprise data picard


all: $(patsubst %,%.build,$(EXE))

%.build:
	gcc $(patsubst %,$*/%,$(FILES)) $(CFLAGS) -o $*/$*

clean: $(patsubst %,%.rm,$(EXE))
	rm -f G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar

%.rm:
	rm -f $*/$*

tar: clean
	tar -cf G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar $(EXE) makefile
