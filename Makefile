################################################################################
#                                                                              #
#                  MAC0422 - Sistemas Operacionais - EP1                       #
#                                                                              #
#                              Makefile for ep1sh                              #
#                                                                              #
#                       Pedro Pereira     - NUSP 9778794                       #
#                       Raphael R. Gusmao - NUSP 9778561                       #
#                                                                              #
################################################################################

MKDIR:=mkdir -p
CD:=cd
CC:=gcc
OBJECT_DIR := obj

.PHONY: clean directory
ep1sh: date.o _aux.o error_handler.o shell.o chown.o
	$(CC) $^ -o $@ -lreadline -Wall

scheduler: scheduler.o _aux.o error_handler.o round_robin.o shortest_job_first.o priority_schedule.o process_queue.o scheduler_methods.o
	$(CC) $^ -o $@ -lm -pthread -Wall

%.o: %.c
	$(CC) $^ -c

clear:
	rm -f *.o

clear_all:
	rm -f *.o ep1sh scheduler trace_*
