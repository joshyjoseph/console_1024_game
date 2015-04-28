# Makefile

all:
	gcc 1024.c -o game_1024 -lncurses
	
clean: 
	rm -rf game_1024
