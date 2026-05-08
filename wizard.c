/*
 * Krysler Bagunu
 * CECS 326
 * Lab 2
 * wizard.c
 * 
 * description: right now im just creating a placeholder for my character classes
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon *dungeon;

int main(void)
{
    int fd; // store the shared memory file descriptor

    // open the shared memory that game.c created, also include a check
    fd = shm_open(dungeon_shm_name, O_RDWR, 0666);

    if (fd == -1)
    {
        printf("Wizard error with shared memory\n");
        return 1;
    }

    // connect the shared memory to the dungeon pointer, also include a check
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (dungeon == MAP_FAILED)
    {
        printf("Wizard error with connecting the shared memory)\n");
        return 1;
    }

    // previews message to show that the game is running
    printf("Wizard started...\n");

    // stay alive when the game is running 
    while (dungeon->running)
    {
        usleep(100000); // helps CPU usage
    }

    // removes Wizard mapping from shared memory
    munmap(dungeon, sizeof(struct Dungeon));
    close(fd);

    printf("Wizard exiting...\n");

    return 0;
}