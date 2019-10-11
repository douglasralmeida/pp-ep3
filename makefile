#Makefile do EP3 - Comunicação Coletiva
#Douglas Rodrigue de Almeida
#
#

PROJECTNAME=ep3
CC=gcc

LFLAGS=
CFLAGS=-fopenmp -std=c99 -c -Wall -Wextra -Wpedantic -Iinclude/
LIBS=-lm -lpthread -fopenmp
DBGFLAGS=-ggdb -fno-inline -fno-omit-frame-pointer
BINDIR=bin
OBJDIR=obj
OBJFILES=main.o
OBJECTS=$(addprefix $(OBJDIR)/, $(OBJFILES))
SOURCEDIR=src
ARGS=25 23 sum 2
ARGS1=25 23 all 2

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	@echo 
	@echo Compilando $<...
	mpicc $(DBGFLAGS) $(CFLAGS) $< -o $@

$(PROJECTNAME): $(OBJECTS) 
	@echo 
	@echo Gerando executavel...
	mpicc $(LFLAGS) -o $(BINDIR)/$@ $^ $(LIBS)

.PHONY: all build clean debug memcheck run

all:
	$(PROJECTNAME)

build:
	@echo 
	@echo Gerando arquivo compactado...
	clean
	tar zcvf ../$(MATRICULA).tar.gz doc/documentacao.pdf $(BINDIR)/* $(OBJDIR)/* include/* $(SOURCEDIR)/* --exclude-vcs

clean:
	@echo 
	@echo Excluindo executavel...
	rm -f $(BINDIR)/$(PROJECTNAME)
	@echo Excluindo objetos...
	rm -f $(OBJECTS)
	
debug:
	gdb --args ./$(BINDIR)/$(PROJECTNAME) $(ARGS)

memcheck:
	valgrind -v --leak-check=yes --track-origins=yes $(BINDIR)/$(PROJECTNAME) $(ARGS)

run:
	mpiexec -n 4 ./$(BINDIR)/$(PROJECTNAME) $(ARGS)

show:
	./$(BINDIR)/$(PROJECTNAME) $(ARGS1)

