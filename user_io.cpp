#include <stdio.h>
#include <string.h>		/* memset() */
#include <stdbool.h>    /* type bool */
#include <unistd.h>     /* for usleep(ms) */
#include "key_tables.h" /* key to freq functions */
#include "synth.h"      /* Class Synth */
#include "main.h"

#if(USE_NCURSES)
#include <ncurses.h>    /* This library is for getting input without hitting return */

/* Ncurses user I/O */
void user_io_ncurses(Synth *ps)
{
    /* Initialize ncurses
     * to permit interactive character input
     *
     * ncurses loop waits for key press
     * If valid key, then initialize Tone structure
     * and refresh ncurses display buffer
     */
    initscr(); /* Start curses mode */
    cbreak();  /* Line buffering disabled */
    noecho(); /* Comment this out if you want to echo characters when typing */
    printw("Welcome to my synthesizer!\n"); /* line 0 */
    printw("Keyboard to piano key mapping is:\n");
    printw("wer yu opl -> F3# to A4#\n");
    printw("asdfghjkl;\' -> F3 to B4\n");
    printw("'.' to stop playing oldest tone\n");
    printw("SpaceBar to quit\n"); /* line 5 */
    printw("\n");            /* line 6 is blank for now */
    printw("Key: ");         /* line 7 */
    refresh();

    char ch = '\0'; /* Init ch to null character */
    while (1) {
        char msg[32];
        ch = getch(); // wait for next keypress
        if (ch == ' ') {
            break;  // quit if space
        }
        else if (ch == '.') {
            /* remove key from voicing keys */
            ps->cmd = CMD_RM_KEY;
            sprintf(msg, "Removed key. New key: ");
            /* wait for callback to execute command */
            while (ps->cmd > 0) 
                usleep(20);
        }
        else if ( !valid_key((int)ch) ) {
            /* not a valid synth key */
            sprintf(msg, "Invalid key. New key: ");
        }
        else {
            /* add key to voicing keys */
            ps->new_freq = key2freq(ch);
            ps->cmd = CMD_ADD_KEY;
            sprintf(msg,  "Key   %1c %3.0f, New key: ", ch, ps->new_freq);
            /* wait for callback to execute command */
            while (ps->cmd > 0) 
                usleep(20);
        }
        /* print key list on line 7 */
        mvprintw(6, 0, "%d voicing", ps->num_keys);
        for (int i=0; i<KEYS_VOICED; i++) {
            mvprintw(7, 10+4*i, " %c", (char)ps->tone[i].key);
        }
        mvprintw(7, 0, "%s", msg);
        refresh(); //display NCurses screen buffer on terminal screen
    }

    endwin();
}

#else

/* fgets() and printf() user I/O */
#define LEN_STR 80

/* Clears screen using ANSI escape sequences. */
void clear()
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

void print_info(Synth *ps, char *msg)
{
    /* User I/O using printf() and getchar() */
    clear();
    printf("Welcome to my synthesizer!\n"); /* line 0 */
    printf("Keyboard to piano key mapping is:\n");
    printf("wer yu opl  -> F3# to A4#\n");
    printf("asdfghjkl;\' -> F3 to B4\n");
    printf("'.' to stop playing oldest tone\n");
    printf("SpaceBar to quit\n"); /* line 6 */
    if (ps->num_keys == 0) {
        printf("\n");            /* line 7 is blank for now */
    }
    else {
        /* print key list on line 7 */
        printf("%d voicing", ps->num_keys);
        for (int i=0; i<KEYS_VOICED; i++) {
            printf(" %c", (char)ps->tone[i].key);
        }  
        printf("\n");    
    }
    printf("%s", msg);         /* line 8 */
    fflush(stdout);
}

void user_io_plain(Synth *ps)
{
    char in_str[LEN_STR]; /* user input string */
    char msg[LEN_STR];
    print_info(ps, "Press key(s) and Enter: ");
    bool done = false;
    while ( !done ) {
        /* read input line, including trailing \n */
        fgets(in_str, LEN_STR, stdin); // wait for input
        int n = strlen(in_str)-1; //don't process \n key
        for (int i=0; i<n; i++) {
            if (in_str[i] == ' ') {
                done = true;
                break;  // break out of for loop and quit while loop
            }
            else if (in_str[i] == '.') {
                /* remove key from voicing keys */
                ps->cmd = CMD_RM_KEY;
                sprintf(msg, "Removed key. New key: ");
                /* wait for callback to execute command */
                while (ps->cmd > 0) 
                    usleep(20);
            }
            else if ( !valid_key((int)in_str[i]) ) {
                /* not a valid synth key */
                sprintf(msg, "Invalid key. New key: ");
            }
            else {
                /* add key to voicing keys */
                ps->new_freq = key2freq(in_str[i]);
                ps->cmd = CMD_ADD_KEY;
                sprintf(msg,  "Key   %1c %3.0f, New key: ", in_str[i], ps->new_freq);
                /* wait for callback to execute command */
                while (ps->cmd > 0) 
                    usleep(20);
            }
        }
        print_info(ps, msg);
    }
    clear();
}
#endif