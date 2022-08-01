/**********************************************************************************

	Terminal Control
	Copyright 2021 Gregg Ink
	This library is released under the zlib license

	This library has no dependencies other than the standard C runtime library
	Note: add the -lpthread to gcc to compile 

***********************************************************************************/
#ifndef TC_H
#define TC_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#ifndef NULL
#define NULL '\0'
#endif

#define TC_NRM  "\x1B[0m"
#define TC_RED  "\x1B[1;31m"
#define TC_GRN  "\x1B[1;32m"
#define TC_YEL  "\x1B[1;33m"
#define TC_BLU  "\x1B[1;34m"
#define TC_MAG  "\x1B[1;35m"
#define TC_CYN  "\x1B[1;36m"
#define TC_WHT  "\x1B[1;37m"

#define TC_B_NRM  "\x1B[0m"
#define TC_B_RED  "\x1B[0;31m"
#define TC_B_GRN  "\x1B[0;32m"
#define TC_B_YEL  "\x1B[0;33m"
#define TC_B_BLU  "\x1B[0;34m"
#define TC_B_MAG  "\x1B[0;35m"
#define TC_B_CYN  "\x1B[0;36m"
#define TC_B_WHT  "\x1B[0;37m"

#define TC_BG_NRM "\x1B[40m"
#define TC_BG_RED " \x1B[41m"
#define TC_BG_GRN "\x1B[42m"
#define TC_BG_YEL "\x1B[43m"
#define TC_BG_BLU "\x1B[44m"
#define TC_BG_MAG "\x1B[45m"
#define TC_BG_CYN "\x1B[46m"
#define TC_BG_WHT "\x1B[47m"

#define tc_clear_screen() puts("\x1B[2J")

#define tc_move_cursor(X, Y) printf("\033[%d;%dH", Y, X)

void tc_get_cols_rows(int *cols, int *rows);

#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")

void tc_echo_off();
void tc_echo_on();

void tc_get_cols_rows(int *cols, int *rows){

	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	*cols = size.ws_col;
	*rows = size.ws_row;

}//tec_get_cols_rows*/

void tc_echo_off(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(1, TCSANOW, &term);

}//tc_echo_off*/

void tc_echo_on(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ECHO;
	tcsetattr(1, TCSANOW, &term);

}//tc_echo_on*/

void tc_canon_on(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ICANON;
	tcsetattr(1, TCSANOW, &term);

}//tc_canon_on*/

void tc_canon_off(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ICANON;
	tcsetattr(1, TCSANOW, &term);

}//tc_canon_off*/

///////////////////////////////////////////////////////
//	Additional code to handle keyboard input
///////////////////////////////////////////////////////

#define TC_KEY_ESCAPE 0x1b
#define TC_KEY_F1 0x1b4f50
#define TC_KEY_F1_TTY 0x1b5b5b41
#define TC_KEY_F2 0x1b4f51
#define TC_KEY_F2_TTY 0x1b5b5b42
#define TC_KEY_F3 0x1b4f52
#define TC_KEY_F3_TTY 0x1b5b5b43
#define TC_KEY_F4 0x1b4f53
#define TC_KEY_F4_TTY 0x1b5b5b44
#define TC_KEY_F5 0x1b5b31357e
#define TC_KEY_F5_TTY 0x1b5b5b45
#define TC_KEY_F6 0x1b5b31377e
#define TC_KEY_F7 0x1b5b31387e
#define TC_KEY_F8 0x1b5b31397e
#define TC_KEY_F9 0x1b5b32307e
#define TC_KEY_F10 0x1b5b32317e
#define TC_KEY_F11 0x1b5b32337e
#define TC_KEY_F12 0x1b5b32347e

#define TC_KEY_ARROW_UP 0x1b5b41
#define TC_KEY_ARROW_DOWN 0x1b5b42
#define TC_KEY_ARROW_LEFT 0x1b5b44
#define TC_KEY_ARROW_RIGHT 0x1b5b43

#define TC_KEY_TAB 0x9
#define TC_KEY_RETURN 0xa
#define TC_KEY_ENTER 0xa

#define TC_KEY_INSERT 0x1b5b327e
#define TC_KEY_HOME 0x1b5b48
#define TC_KEY_PAGE_UP 0x1b5b357e
#define TC_KEY_DELETE 0x1b5b337e
#define TC_KEY_END 0x1b5b46
#define TC_KEY_PAGE_DOWN 0x1b5b367e

#define TC_KEY_EURO 0xffffffffffe181ac
#define TC_KEY_GBP 0xffffffffffffc1a3

#define RAW_INPUT_SIZE 120

typedef struct _raw_input raw_input;
typedef struct _tc_inp ti;

ti* tc_init_input(void (*func)(), void *data);

void t_process_keycode(ti *t);
void t_process_thread(ti *t);

struct _raw_input{

	struct timespec tp;
	char c;

};

struct _tc_inp{

	raw_input ri[RAW_INPUT_SIZE];

	void (*key_cb) (uint64_t key, void *data);
	void *data;

};

ti* tc_init_input( void (*func)(), void *data){

	ti *t = (ti *) malloc(sizeof(ti));
	memset(t, 0, sizeof(ti));

	tc_canon_off();

	t->key_cb = func;
	t->data = data;

	pthread_t thid, thid2;
	pthread_create(&thid, NULL, (void *) &t_process_thread, (void *) t);
	pthread_create(&thid2, NULL, (void *) &t_process_keycode, (void *) t);

	return t;

}//tc_init_input*/

void t_process_keycode(ti *t){

	int i = 0;
	uint64_t key = 0;
	uint64_t time;
	uint64_t next_time;
	void (*key_cb) (uint64_t c, void *data);

	while(1){
		while( !((t->ri[i]).tp.tv_nsec) ) {
			usleep(10000);
		}
		usleep(100);

		key = (t->ri[i]).c;
		time = ((t->ri[i]).tp.tv_sec) * 1000000000;
		time += ((t->ri[i]).tp.tv_nsec);
		(t->ri[i]).tp.tv_nsec = 0;

		i += 1;
		if(i >= RAW_INPUT_SIZE){
			i = 0;
		}

		if( (t->ri[i]).tp.tv_nsec ){
			next_time = ((t->ri[i]).tp.tv_sec) * 1000000000;
			next_time += ((t->ri[i]).tp.tv_nsec);
			while(next_time - time < 100000){	//100,000 nanoseconds
				key <<= 8;
				key += (t->ri[i]).c;
				time = next_time;
				(t->ri[i]).tp.tv_nsec = 0;
				i += 1;
				if(i >= RAW_INPUT_SIZE){
					i = 0;
				}

				next_time = ((t->ri[i]).tp.tv_sec) * 1000000000;
				next_time += ((t->ri[i]).tp.tv_nsec);
			}
		}

		key_cb = t->key_cb;
		if(key_cb){
			(*key_cb)(key, t->data);
		}
		key = 0;

	}

}//t_process_keycode*/

void t_process_thread(ti *t){

	//if we get two bytes within 100,000 nanosecs, it is part of the same keystroke
	//every keystroke, up to 6 scan codes

	int ri_index = 0;
	int n = 1;

	while(n){
		//is there non-blocking version of fread or read?
		n = fread( &((t->ri[ri_index]).c) , 1, 1, stdin);
		clock_gettime(CLOCK_MONOTONIC, &((t->ri[ri_index]).tp) );
		ri_index += 1;
		if(ri_index >= RAW_INPUT_SIZE){
			ri_index = 0;
		}
	}

}//t_process_thread*/

#endif

