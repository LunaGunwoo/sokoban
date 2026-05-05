#include <stdio.h>
#include <termio.h>

int getch(void);
void show_inital_screen();
void show_option_screen();
void show_name_screen();

int main(void) {
  char maps[3][30][30] = {{
                              "....#####..........",
                              "....#...#..........",
                              "....#$..#..........",
                              "..###..$##.........",
                              "..#..$.$.#.........",
                              "###.#.##.#...######",
                              "#...#.##.#####..OO#",
                              "#.$..$..........OO#",
                              "#####.###.#@##..OO#",
                              "....#.....#########",
                              "....#######........",
                          },
                          {
                              "############..",
                              "#OO..#.....###",
                              "#OO..#.$..$..#",
                              "#OO..#$####..#",
                              "#OO....@.##..#",
                              "#OO..#.#..$.##",
                              "######.##$.$.#",
                              "..#.$..$.$.$.#",
                              "..#....#.....#",
                              "..############",
                          },
                          {
                              "........########.",
                              "........#.....@#.",
                              "........#.$#$.##.",
                              "........#.$..$#..",
                              "........##$.$.#..",
                              "#########.$.#.###",
                              "#OOOO..##.$..$..#",
                              "##OOO....$..$...#",
                              "#OOOO..##########",
                              "########.........",
                          }};
  char option;
  char name[3];

  show_inital_screen();
  show_option_screen();
  scanf("%c", &option);
  show_name_screen();
  scanf("%c", name);
  return 0;
}

int getch(void) {
  int ch;

  struct termios buf;
  struct termios save;

  tcgetattr(0, &save);
  buf = save;

  buf.c_lflag &= ~(ICANON | ECHO);
  buf.c_cc[VMIN] = 1;
  buf.c_cc[VTIME] = 0;

  tcsetattr(0, TCSAFLUSH, &buf);

  ch = getchar();
  tcsetattr(0, TCSAFLUSH, &save);

  return ch;
}

void show_inital_screen() {
  printf("=======================================\n");
  printf("       S   O   K   O   B   A   N       \n");
  printf("=======================================\n");
  printf("\n");
  printf("n : New Game\n");
  printf("1~3 : Level Number\n");
  printf("\n\n");
}

void show_option_screen() { printf("Input option : "); }
void show_name_screen() { printf("Input your name : "); }