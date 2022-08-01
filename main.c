#include <time.h>
#include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "tc.h"


#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))
#define cursorup(x) printf("\033[%dA", (x))
#define cursordown(x) printf("\033[%dB", (x))

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a
#define KEY_UP      0x0105
#define KEY_DOWN    0x0106
#define KEY_LEFT    0x0107
#define KEY_RIGHT   0x0108
// define the key code of c character
#define KEY_C_A     0x0061
typedef struct 
{
    int neighbors;
    int shown;
    char state;
    int marked;
}field;
typedef struct 
{
    int row,col;
    field data[100][100];
}board;
static struct termios term, oterm;

static int getch(void);
static int kbhit(void);
static int kbesc(void);
static int kbget(void);

static int getch(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    return c;
}

static int kbhit(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    if (c != -1) ungetc(c, stdin);
    return ((c != -1) ? 1 : 0);
}

static int kbesc(void)
{
    int c;

    if (!kbhit()) return KEY_ESCAPE;
    c = getch();
    if (c == '[') {
        switch (getch()) {
            case 'A':
                c = KEY_UP;
                break;
            case 'B':
                c = KEY_DOWN;
                break;
            case 'C':
                c = KEY_LEFT;
                break;
            case 'D':
                c = KEY_RIGHT;
                break;
            default:
                c = 0;
                break;
        }
    } else {
        c = 0;
    }
    if (c == 0) while (kbhit()) getch();
    return c;
}

static int kbget(void)
{
    int c;

    c = getch();
    return (c == KEY_ESCAPE) ? kbesc() : c;
}

// random position of bomb


void generate_board(board *b){
    int i,j;
    for(i=0;i<100;i++){
        for(j=0;j<100;j++){
             b->data[i][j].state=' ';
        }
    }
}
void calcule_neighbors(board *b)
{
    int i,j;
    for(i=0;i<b->row;i++)
    {
        for(j=0;j<b->col;j++)
        {
            if(b->data[i][j].state=='*')
            {
                if(i-1>=0 && j-1>=0) b->data[i-1][j-1].neighbors++;
                if(i-1>=0) b->data[i-1][j].neighbors++;
                if(i-1>=0 && j+1<b->col) b->data[i-1][j+1].neighbors++;
                if(j-1>=0) b->data[i][j-1].neighbors++;
                if(j+1<b->col) b->data[i][j+1].neighbors++;
                if(i+1<b->row && j-1>=0) b->data[i+1][j-1].neighbors++;
                if(i+1<b->row) b->data[i+1][j].neighbors++;
                if(i+1<b->row && j+1<b->col) b->data[i+1][j+1].neighbors++;
            }
        }
    }
    for (int i = 0; i < b->row; i++)
    {
        for (int j = 0; j < b->col; j++)
        {
            if(b->data[i][j].state==' ')
            {
                if(b->data[i][j].neighbors==0)
                {
                    b->data[i][j].state=' ';
                }
                else
                {
                    b->data[i][j].state=b->data[i][j].neighbors+'0';
                }

            }
        }
        
    }
    
}
// set field to bomb
void field_to_bomb(board *b,int row,int col)
{
    b->data[row][col].state='*';
}
void set_shown(board *b,int c_row,int c_col)
{
    b->data[c_row][c_col].shown=1;
}
/* 
   # # #
   # # #
   # # #

    0 1
    0 0
*/

// open all fields doesnt have bomb around with number of bombs around
void reveal_field(board *b,int row,int col)
{
    b->data[row][col].shown=1;
    if(b->data[row][col].state==' ' )
    {
        b->data[row][col].shown=1;
        if(row-1>=0 && col-1>=0 && b->data[row][col-1].shown !=1) reveal_field(b,row-1,col-1);
        if(row-1>=0 && b->data[row-1][col].shown !=1) reveal_field(b,row-1,col );
        if(row-1>=0 && col+1<b->col && b->data[row-1][col+1].shown !=1) reveal_field(b,row-1,col+1);
        if(col-1>=0 && b->data[row][col-1].shown !=1) reveal_field(b,row,col-1);
        if(col+1<b->col && b->data[row][col+1].shown !=1) reveal_field(b,row,col+1);
        if(row+1<b->row && col-1>=0 && b->data[row+1][col-1].shown !=1) reveal_field(b,row+1,col-1);
        if(row+1<b->row && b->data[row+1][col].shown !=1) reveal_field(b,row+1,col);
        if(row+1<b->row && col+1<b->col && b->data[row+1][col+1].shown !=1) reveal_field(b,row+1,col+1);

    }
}
void print_board(board b){
    int i,j;
    for(i=0;i<b.row;i++){
        for(j=0;j<b.col;j++){

            if(b.data[i][j].shown == 1)
            {
                if(b.data[i][j].state=='*')
                {
                    printf(" %s*%s ",TC_B_GRN,TC_NRM);
                }
                else
                {
                    printf(" %s%c%s ",TC_YEL,b.data[i][j].state,TC_NRM);
                }
            }else printf(" %s#%s ",TC_YEL,TC_NRM);
        }
        printf("\n");
    }
}
void show_all(board *b){
    int i,j;
    for(i=0;i<b->row;i++){
        for(j=0;j<b->col;j++){
            b->data[i][j].shown=1;
        }
    }
}
int main(){
    printf("%smine swiper%s\n",TC_B_GRN,TC_NRM);
    board b;
    b.row=20;
    b.col=20;
    generate_board(&b);

    // set 20% of field to bomb
    int i,j;
    for(i=0;i<b.row;i++){
        for(j=0;j<b.col;j++){
            b.data[i][j].shown=0;
            if(rand()%100<20)
            {
                field_to_bomb(&b,i,j);
            }
        }
    }
    calcule_neighbors(&b);
    tc_clear_screen();
    tc_move_cursor(0,0);
    print_board(b);
    tc_move_cursor(2,0);
    int c;
    int curr_cursor_x=0,curr_cursor_y=0;
    int curr_cursor_x_char = 0,curr_cursor_y_char = 0;
    while (1) {

        c = kbget();
        if ( c == KEY_ESCAPE ) {
            break;
        } else
        if (c == KEY_RIGHT) {
            curr_cursor_x_char = curr_cursor_x_char-3;
          if(curr_cursor_x>0)
            {
                curr_cursor_x--;
            }
            cursorbackward(3);
        } else
        if (c == KEY_LEFT) {
            if(curr_cursor_x<b.col-1)
            {
                curr_cursor_x++;
                cursorforward(3);
            }
        }else
        if(c == KEY_UP)
        {
            if(curr_cursor_y>0)
            {
                curr_cursor_y--;
                cursorup(1);
            }

        }else
        if (c == KEY_DOWN)
        {
            if(curr_cursor_y<b.row-1)
            {
                curr_cursor_y++;
                cursordown(1);
            }
        }else if (c == KEY_ENTER)
        {
            if(b.data[curr_cursor_y][curr_cursor_x].state=='*')
            {
                tc_clear_screen();
                show_all(&b);
                tc_move_cursor(0,0);
                print_board(b);
                printf("%sYou lose%s\n",TC_B_RED,TC_NRM);
                break;
            }
            else
            {
                set_shown(&b,curr_cursor_y,curr_cursor_x);
                reveal_field(&b,curr_cursor_y,curr_cursor_x);
                tc_clear_screen();
                tc_move_cursor(0,0);
                print_board(b);
                printf("%d %d",curr_cursor_y,curr_cursor_x);
                printf("%d",b.data[curr_cursor_y][curr_cursor_x].neighbors);
                tc_move_cursor(2 +curr_cursor_x*3,curr_cursor_y+1);
                // curr_cursor_x =0;
                // curr_cursor_y =0;

            }
        } else
        if(c == 'm'){
           if(b.data[curr_cursor_y][curr_cursor_x].marked !=1)
           {
            printf("%s?%s",TC_B_YEL,TC_NRM);
            b.data[curr_cursor_y][curr_cursor_x].marked=1;
            cursorbackward(1);
           }
        else{
            b.data[curr_cursor_y][curr_cursor_x].marked=0;
            printf("%s#%s",TC_B_YEL,TC_NRM);
            cursorbackward(1);
           }
        }            
    }
    return 0;
}