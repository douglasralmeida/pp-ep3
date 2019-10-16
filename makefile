#Makefile do EP3 - Comunicação Coletiva
#Douglas Rodrigue de Almeida
#
#

PROJECTNAME=ep3
CC=mpicc

CFLAGS=-c -std=c99 -Wall -Wextra -Wpedantic -Iinclude/
DBGFLAGS=-ggdb -fno-inline -fno-omit-frame-pointer
LIBS=-lm -lpthread
BINDIR=bin
OBJDIR=obj
OBJFILES=main.o
OBJECTS=$(addprefix $(OBJDIR)/, $(OBJFILES))
SOURCEDIR=src

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	@echo 
	@echo Compilando $<...
	$(CC) $(DBGFLAGS) $(CFLAGS) $< -o $@

$(PROJECTNAME): $(OBJECTS) 
	@echo 
	@echo Gerando executavel...
	$(CC) $(LFLAGS) -o $(BINDIR)/$@ $^ $(LIBS)

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
	mpiexec -n 7 gdb -ex run --args ./$(BINDIR)/$(PROJECTNAME)

memcheck:
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes mpiexec -n 7 ./$(BINDIR)/$(PROJECTNAME)

run:
	mpiexec -n 7 ./$(BINDIR)/$(PROJECTNAME)

show:
	./$(BINDIR)/$(PROJECTNAME)

