# Makefile for Razgriz Shell
# Created by:
# 	Pedro Pereira 					9778794
# 	Raphael Gusmão					9778561
MKDIR:=mkdir -p
CD:=cd
CC:=gcc
OBJECT_DIR := obj

.PHONY: clean directory
ep1sh: date.o aux.o error_handler.o shell.o chown.o
	$(CC) -lreadline $^ -o $@

%.o: %.c
	$(CC) $^ -c

clear:
	rm -f *.o

clear_all:
	rm -f *.o ep1sh
