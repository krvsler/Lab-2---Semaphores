/*
 * Krysler Bagunu
 * CECS 326
 * Lab 2
 * game.c
 * 
 * description: this is where the game/dungeon will launch and run. this is the main setup file
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

int main(void)
{
    int fd; // store the shared memory file descriptor
    struct Dungeon *dungeon; 

    // process ids
    pid_t barbarian_pid; 
    pid_t rogue_pid;
    pid_t wizard_pid;

    // semaphores for the two levers at the end of the dungeon
    sem_t *dungeon_lever_one;
    sem_t *dungeon_lever_two;

    // create a shared memory named DungeonMem with a check that shared memory is big enough for the Dungeon struct
    fd = shm_open("DungeonMem", O_CREAT | O_RDWR, 0666);

    if(fd == -1) 
    {
        printf("Error with shared memory\n");
        return 1;
    }
    
    if(ftruncate(fd, sizeof(struct Dungeon)) == -1)
    {
        printf("Error with setting the size of shared memory for the Dungeon struct\n");
        return 1;
    }

    // map the shared memory to the dungeon pointer that allows to use the pointer to access the struct. also add a check to make sure it works
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (dungeon == MAP_FAILED) 
    {
        printf("Error with mapping the shared memory to the dungeon pointer\n");
        return 1;
    }

    // store the game pid in memory
    dungeon->dungeonPID = getpid();
    dungeon->running = true;

    // start the barbarian, wizard, and rogue programs
    barbarian_pid = fork();

    if (barbarian_pid == 0)
    {
        execl("./barbarian", "barbarian", NULL);
        printf("Error occurred while executing barbarian\n");
        exit(1);
    }

    wizard_pid = fork();
    if (wizard_pid == 0)
    {
        execl("./wizard", "wizard", NULL);
        printf("Error occurred while executing wizard\n");
        exit(1);
    }

    rogue_pid = fork();
    if (rogue_pid == 0)
    {
        execl("./rogue", "rogue", NULL);
        printf("Error occurred while executing rogue\n");
        exit(1);
    }


    // call RunDungeon with the process ids
    RunDungeon(wizard_pid, rogue_pid, barbarian_pid);
    dungeon->running = false; // stops after dungeon is finished running

    // clean up shared memory after the dungeon is done
    waitpid(barbarian_pid, NULL, 0);
    waitpid(wizard_pid, NULL, 0);
    waitpid(rogue_pid, NULL, 0);
    munmap(dungeon, sizeof(struct Dungeon));
    close(fd);
    shm_unlink(dungeon_shm_name);

    return 0;
}