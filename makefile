LOGIN1 = ls30687
LOGIN2 = ls31285
GRUP = 18
FASE = 3
EXE = enterprise data picard


all: $(patsubst %,%.build,$(EXE))

%.build:
	$(eval MAIN := $(shell sed -n 's/MAIN = //p' $*/makefile))
	$(eval MODULES := $(shell sed -n 's/MODULES = //p' $*/makefile))
	$(eval CFLAGS := $(shell sed -n 's/CFLAGS = //p' $*/makefile))
	gcc $*/$(MAIN) $(patsubst %,$*/%.c,$(MODULES)) $(CFLAGS) -o $*/$*

clean: $(patsubst %,%.rm,$(EXE))
	rm -f G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar

%.rm:
	rm -f $*/$*

tar: clean
	tar -cf G$(GRUP)_F$(FASE)_$(LOGIN1)_$(LOGIN2).tar $(EXE) makefile
