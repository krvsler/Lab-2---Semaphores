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
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon *dungeon; // global var
sem_t *lever_two; // lever semaphore for the treasure room

// signal handler to signal the wizard to decode the spell
void handle_signal(int sig)
{
    int shift;
    char key;
    int i;
    int j;
    char current;
    
    if (sig == DUNGEON_SIGNAL)
    {
        key = dungeon->barrier.spell[0];

        // get the shift value using the key and the first letter of the spell
        if (key >= 'a' && key <= 'z')
        {
            shift = 'h' - key; // compared the lowercase key with lowercase h to get the decode shift
        }
        else
        {
            shift = 'N' - key; // compared the uppercase keys with uppercase N to get the decode shift
        }

        i = 1; // i is the index for the encoded spell, starts at 1 because the first character of the spell is the key and not part of the spell
        j = 0; // j is the index for the decoded spell, starts at 0 because the first character of the spell is the key and not part of the spell

        // decode the spell
        while (dungeon->barrier.spell[i] != '\0' && j < SPELL_BUFFER_SIZE - 1) 
        {
            current = dungeon->barrier.spell[i]; 

            // check if current character is an underscore
            if (current == '_')
            {
                current = ' '; // replace underscores with spaces in the decoded spell
            }

            // check if the current character is a lowercase letter
            if(current >= 'a' && current <= 'z')
            {
                current = current + shift;
                while (current < 'a')
                {
                    current = current + 26; // wrap around if the shift goes before 'a'
                }

                while (current > 'z')
                {
                    current = current - 26; // wrap around if the shift goes after 'z'
                }
            }

            // check if the current character is an uppercase letter
            else if (current >= 'A' && current <= 'Z')
            {
                current = current + shift;
                while (current < 'A')
                {
                    current = current + 26;
                }
                
                while (current > 'Z')
                {
                    current = current - 26;
                }
            }

            dungeon->wizard.spell[j] = current; // store the decoded character in the wizard spell
            i++;
            j++;
        }

        dungeon->wizard.spell[j] = '\0'; // terminate the decoded spell when reached the end
    }

    else if (sig == SEMAPHORE_SIGNAL)
    {
        // hold the lever
        sem_wait(lever_two);

        // wait until the rogue gets all the treasure
        while (dungeon->treasure[3] == '\0') 
        {
            usleep(100000);
        }

        // release the lever
        sem_post(lever_two);
    }
}

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
        close(fd);
        return 1;
    }

    // run handle_signal when the wizard receives the dungeon signal
    signal(DUNGEON_SIGNAL, handle_signal);

    // run handle_signal when the wizard receives the semaphore signal
    signal(SEMAPHORE_SIGNAL, handle_signal);

    // open lever for the treasure room
    lever_two = sem_open(dungeon_lever_two, 0);

    // previews message to show that the game is running
    printf("Wizard started...\n");

    // stay alive when the game is running 
    while (dungeon->running)
    {
        usleep(100000); // helps CPU usage
    }

    // close the lever semaphore
    sem_close(lever_two);

    // removes Wizard mapping from shared memory
    munmap(dungeon, sizeof(struct Dungeon));
    close(fd);

    printf("Wizard exiting...\n");

    return 0;
}