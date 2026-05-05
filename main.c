#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <termio.h>
#define SIZE 30
#define MAX_LEVEL 3

int getch(void);
void show_inital_screen();
void show_option_screen();
void show_name_screen();
void show_complete_screen();
void flush_stdin_line();
bool validate_map(char[SIZE][SIZE]);
void show_playing_map(char[], int, char[SIZE][SIZE]);

int main(void) {
  char maps[MAX_LEVEL][SIZE][SIZE] = {{
                                          "    #####          ",
                                          "    #   #          ",
                                          "    #$  #          ",
                                          "  ###  $##         ",
                                          "  #  $ $ #         ",
                                          "### # ## #   ######",
                                          "#   # ## #####  OO#",
                                          "# $  $          OO#",
                                          "##### ### #@##  OO#",
                                          "    #     #########",
                                          "    #######        ",
                                      },
                                      {
                                          "############  ",
                                          "#OO  #     ###",
                                          "#OO  # $  $  #",
                                          "#OO  #$####  #",
                                          "#OO    @ ##  #",
                                          "#OO  # #  $ ##",
                                          "###### ##$ $ #",
                                          "  # $  $ $ $ #",
                                          "  #    #     #",
                                          "  ############",
                                      },
                                      {
                                          "        ######## ",
                                          "        #     @# ",
                                          "        # $#$ ## ",
                                          "        # $  $#  ",
                                          "        ##$ $ #  ",
                                          "######### $ # ###",
                                          "#OOOO  ## $  $  #",
                                          "##OOO    $  $   #",
                                          "#OOOO  ##########",
                                          "########         ",
                                      }};
  // >>> validate map >>>
  for (int k = 0; k < MAX_LEVEL; k++) {
    if (!validate_map(maps[k])) {
      printf("Wrong level %d map\n", k + 1);
      return 0;
    }
  }
  // <<< validate map <<<

  char option;
  char name[4];
  int playing_level;

  show_inital_screen();
  show_option_screen();
  scanf("%c", &option);
  option = tolower(option);
  show_name_screen();
  scanf("%s", name);
  flush_stdin_line();

  switch (option) {
    case 'n':
    case 'f':
      // TODO
      break;
      return 0;
    case '1':
    case '2':
    case '3':
      playing_level = option - '1';
      break;
  }

  char playing_map[SIZE][SIZE];
  bool box_dest_map[SIZE][SIZE];
  int left_box_cnt;
  int player_y, player_x;
  bool is_complete_level;
  // >>> maps -> playing_map copy & player 위치 준비 >>>
  // only depend on (playing_level)
  left_box_cnt = 0;
  is_complete_level = false;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      playing_map[i][j] = maps[playing_level][i][j];
      if (playing_map[i][j] == '@') {
        player_y = i, player_x = j;
      }
      if (playing_map[i][j] == 'O') {
        box_dest_map[i][j] = true;
        left_box_cnt++;
      } else {
        box_dest_map[i][j] = false;
      }
    }
  }
  // <<< maps -> playing_map copy & player 위치 준비 <<<

  bool is_first_game = true;
  bool is_gone_next_level = false;
  while (1) {
    show_playing_map(name, playing_level, playing_map);

    // >>> 참고 메시지 >>>
    if (is_first_game) {
      is_first_game = false;
      printf("Welcome %s\n", name);
    } else if (is_gone_next_level) {
      is_gone_next_level = false;
      printf("You are in the level %d, now.\n", playing_level);
    }
    // <<< 참고 메시지 <<<

    // >>> Level Clear 한 경우 >>>
    if (is_complete_level) {
      if (playing_level + 1 >= MAX_LEVEL) {  // 모든 Level clear 한 경우
        printf("No more level\n");
        printf("Good bye\n");
        return 0;
      }

      char user_input;
      show_complete_screen();
      scanf("%c", &user_input);
      user_input = tolower(user_input);
      flush_stdin_line();

      bool go_next_level = (user_input == 'y') ? true : false;
      if (go_next_level) {  // >>> next level로 이동 >>>
        playing_level++;
        // >>> maps -> playing_map copy & player 위치 준비 >>>
        // only depend on (playing_level)
        left_box_cnt = 0;
        is_complete_level = false;
        for (int i = 0; i < SIZE; i++) {
          for (int j = 0; j < SIZE; j++) {
            playing_map[i][j] = maps[playing_level][i][j];
            if (playing_map[i][j] == '@') {
              player_y = i, player_x = j;
            }
            if (playing_map[i][j] == 'O') {
              box_dest_map[i][j] = true;
              left_box_cnt++;
            } else {
              box_dest_map[i][j] = false;
            }
          }
        }
        // <<< maps -> playing_map copy & player 위치 준비 <<<
        continue;
      }  // <<< next level로 이동 <<<
      else {
        printf("Good bye!!\n");
        return 0;
      }
    }
    // <<< Level Clear 한 경우 <<<

    // >>> user 입력 >>>
    int op = tolower(getch());
    int dy = 0, dx = 0;
    switch (op) {
      case 'h':
        dx = -1;
        break;
      case 'j':
        dy = 1;
        break;
      case 'k':
        dy = -1;
        break;
      case 'l':
        dx = 1;
        break;
    }
    // <<< user 입력 <<<

    if (dx != 0 || dy != 0) {  // >>> 이동 조작키를 눌렀을 때 >>>
      // (이동 관련은 if문 안에서만 처리하기)
      int ny = player_y + dy;
      int nx = player_x + dx;
      if (playing_map[ny][nx] == '#') continue;
      if (playing_map[ny][nx] == '$') {
        int box_ny = ny + dy;
        int box_nx = nx + dx;
        char next_block = playing_map[box_ny][box_nx];
        if (next_block == '#' || next_block == '$')
          continue;  // early return 박스 이동 불가한 경우 바로 다시 키 입력받기

        // >>> 박스 이동 처리 >>>
        if (box_dest_map[box_ny][box_nx] != box_dest_map[ny][nx]) {
          if (box_dest_map[box_ny][box_nx])
            left_box_cnt--;
          else
            left_box_cnt++;
        }
        playing_map[box_ny][box_nx] = '$';
        // <<< 박스 이동 처리 <<<
      }
      playing_map[ny][nx] = '@';
      playing_map[player_y][player_x] =
          (box_dest_map[player_y][player_x]) ? 'O' : ' ';  // 이동 전 user 위치
      player_y = ny;
      player_x = nx;

      if (left_box_cnt == 0)
        is_complete_level = true;  // Level Clear는 while 문 다시 시작할 때 처리
    }  // <<< 이동 조작키를 눌렀을 때 <<<
  }
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
  printf("f : File load\n");
  printf("1~3 : Level Number\n");
  printf("\n\n");
}

void show_option_screen() { printf("Input option : "); }
void show_name_screen() { printf("Input your name : "); }
void show_complete_screen() { printf("Good job! Continue (N/Y) "); }
void flush_stdin_line() {
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

bool validate_map(char map[SIZE][SIZE]) {
  int box_cnt = 0, storage_cnt = 0;
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < SIZE; j++) {
      char block = map[i][j];
      if (block == '$')
        box_cnt++;
      else if (block == 'O')
        storage_cnt++;
    }
  if (storage_cnt == box_cnt) return true;
  return false;
}

void show_playing_map(char name[], int level, char map[SIZE][SIZE]) {
  printf("================\n");
  printf(" %s in Level %d \n", name, level);
  printf("================\n");

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }
}