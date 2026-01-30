CC := gcc
YACC := bison
LEX := flex

WARN := -Wall -Werror
INTERNAL_CFLAGS := $(WARN) -std=gnu23 -MMD
override CFLAGS += $(INTERNAL_CFLAGS)

SRC_FILES := $(wildcard *.c)
OBJ_FILES := $(SRC_FILES:.c=.o)

LIBS := -lm -lffi

all: hlim

hlim: $(OBJ_FILES) gram.tab.o lex.yy.o
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

lex.yy.o: lex.yy.c gram.tab.h
	$(CC) -c $< $(CFLAGS) -o $@ \
		-Wno-implicit-function-declaration \
		-Wno-unused-function

%.o: %.c gram.tab.h
	$(CC) -c $< $(CFLAGS) -o $@

gram.tab.h: gram.tab.c
gram.tab.c: gram.y hlim.h
	$(YACC) -d $< -Werror

lex.yy.c: lex.l
	$(LEX) $<

clean: clean-deps clean-objs
	rm -f hlim gram.tab.* lex.yy.c

clean-docs:
	rm -f *.pdf docs.aux docs.log

clean-deps:
	rm -f *.d

clean-objs:
	rm -f *.o

TEST_FILES := $(wildcard tests/*)
test:
	@for file in $(TEST_FILES); do \
		echo "$$file"; \
		./hlim < ./$$file || { echo "$$file failed"; exit 1; }; \
	done

HLIM-LANGUAGE-MANUAL.pdf: docs/docs.tex
	pdflatex $<
	mv docs.pdf $@

-include $(OBJ_FILES:.o=.d)

.PHONY: clean all

