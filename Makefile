# Lab 2 Makefile - CECS 326
#
# This will make the game launcher, and the three character executables (barbarian, wizard, rogue).

# builds ALL executables when 'make' is run
all: game barbarian wizard rogue

# builds the game launcher
game: game.c dungeon.o
	gcc -Wall -Wextra -g game.c dungeon.o -o game -lrt -pthread 

# builds barbarian executable
barbarian: barbarian.c
	gcc -Wall -Wextra -g barbarian.c -o barbarian -lrt -pthread

# builds wizard executable
wizard: wizard.c
	gcc -Wall -Wextra -g wizard.c -o wizard -lrt -pthread

# builds rogue executable
rogue: rogue.c
	gcc -Wall -Wextra -g rogue.c -o rogue -lrt -pthread

# cleans up the directory by removing ALL executables
clean:
	rm -f game barbarian wizard rogue