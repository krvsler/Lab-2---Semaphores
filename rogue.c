/*
 * Krysler Bagunu
 * CECS 326
 * Lab 2
 * rogue.c
 * 
 * description: rogue class where they try to pick the lock and get the treasure
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon *dungeon; //global var

// signal handler to signal the rogue to pick the lock
void handle_signal(int sig)
{
    // implement binary search to get the pick value for the rogue to pick the lock
    float low;
    float high;
    float middle;

    if (sig == DUNGEON_SIGNAL)
    {
        low = 0.0;
        high = 100.0;

        // keep picking while trap is locked
        while (dungeon->trap.direction != '-')
        {
            middle = (low + high) / 2.0; // get middle value
            dungeon->rogue.pick = middle; // store the pick value in the shared memory for the rogue

            // allow the game to check the pick value and update trap
            usleep(TIME_BETWEEN_ROGUE_TICKS);

            // if the pick needs to go up
            if (dungeon->trap.direction == 'u')
            {
                low = middle;
            }

            // if the pick needs to go down
            else if (dungeon->trap.direction == 'd')
            {
                high = middle;
            }

            // if the pick is correct
            else if (dungeon->trap.direction == '-')
            {
                break;
            }            
        }
    }
}

int main(void)
{
    int fd; // store the shared memory file descriptor

    // open the shared memory that game.c created, also include a check
    fd = shm_open(dungeon_shm_name, O_RDWR, 0666);

    if (fd == -1)
    {
        printf("Rogue error with shared memory\n");
        return 1;
    }

    // connect the shared memory to the dungeon pointer, also include a check
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (dungeon == MAP_FAILED)
    {
        printf("Rogue error with connecting the shared memory)\n");
        close(fd);
        return 1;
    }

    // run handle_signal when the rogue receives the dungeon signal
    signal(DUNGEON_SIGNAL, handle_signal);

    // previews message to show that the game is running
    printf("Rogue started...\n");

    // stay alive when the game is running 
    while (dungeon->running)
    {
        usleep(100000); // helps CPU usage
    }

    // removes rogue mapping from shared memory
    munmap(dungeon, sizeof(struct Dungeon));
    close(fd);

    printf("Rogue exiting...\n");

    return 0;
}