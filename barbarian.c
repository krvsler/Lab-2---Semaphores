/*
 * Krysler Bagunu
 * CECS 326
 * Lab 2
 * barbarian.c
 * 
 * description: barbarian class where they attack the monster and hold the lever.
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon *dungeon; // need to be global so that the signal handler can access it
sem_t *lever_one; // lever semaphore for the treasure room

// signal handler to signal the barbarian to attack or hold the lever
void handle_signal(int sig)
{
    if (sig == DUNGEON_SIGNAL)
    {
        dungeon->barbarian.attack = dungeon->enemy.health; // when the barbarian receives the signal, they attack the enemy
    }
    else if (sig == SEMAPHORE_SIGNAL)
    {
        // hold the lever
        sem_wait(lever_one);

        // wait until the rogue gets all the treasure
        while (dungeon->treasure[3] == '\0') 
        {
            usleep(100000);
        }

        // release the lever
        sem_post(lever_one);
    }
}


int main(void)
{
    int fd; // store the shared memory file descriptor

    // open the shared memory that game.c created, also include a check
    fd = shm_open(dungeon_shm_name, O_RDWR, 0666);

    if (fd == -1)
    {
        printf("Barbarian error with shared memory\n");
        return 1;
    }

    // connect the shared memory to the dungeon pointer, also include a check
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (dungeon == MAP_FAILED)
    {
        printf("Barbarian error with connecting the shared memory)\n");
        close(fd);
        return 1;
    }

    // run handle_signal when the barbarian receives the dungeon signal
    signal(DUNGEON_SIGNAL, handle_signal);

    // run handle_signal when the barbarian receives the semaphore signal
    signal(SEMAPHORE_SIGNAL, handle_signal);

    // open lever for the treasure room
    lever_one = sem_open(dungeon_lever_one, 0);

    // previews message to show that the game is running
    printf("Barbarian started...\n");

    // stay alive when the game is running 
    while (dungeon->running)
    {
        usleep(100000); // helps CPU usage
    }

    // close the lever semaphore
    sem_close(lever_one);

    // removes barbarian mapping from shared memory.
    munmap(dungeon, sizeof(struct Dungeon));
    close(fd);

    printf("Barbarian exiting...\n");

    return 0;
}