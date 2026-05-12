#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
void show_playing_map(char[], int, int, int, char[SIZE][SIZE]);
void show_help();
void copy_map(char[SIZE][SIZE], char[SIZE][SIZE], int, int);

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
      playing_level = 0;
      break;
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
  // >>> play에서 필요한 변수들 (SET_PLAYING_MAP_BY_PLAYING_LEVEL 에서 사용하는
  // 변수들) >>>
  char playing_map[SIZE][SIZE];
  bool box_dest_map[SIZE][SIZE];
  int fitted_map_width, fitted_map_height;
  int left_box_cnt;
  int player_y, player_x;
  bool is_complete_level;
  int moves_cnt = 0;
  int op;
  // <<< play에서 필요한 변수들 <<<

  // >>> Undo 기능을 위한 변수 선언 >>>
  char history_map[5][SIZE][SIZE];
  int history_player_y[5];
  int history_player_x[5];
  int history_left_box_cnt[5];
  int undo_count = 0;
  int remain_undo_cnt = 5;
  // <<< Undo 기능을 위한 변수 선언 <<<

  // >>> 기타 명령어나 참고 메시지에 필요한 변수들 >>>
  bool is_first_game = true;
  bool is_gone_next_level = false;
  bool is_showing_help = false;
  bool is_again = false;
  bool is_new = false;
  bool is_undo = false;
  // <<< 기타 명령어나 참고 메시지에 필요한 변수들 <<<

SET_PLAYING_MAP_BY_PLAYING_LEVEL:
  // >>> maps -> playing_map copy & player 위치 준비 >>>
  // only depend on (playing_level)
  for (int i = 0; i < SIZE; i++)
    if (maps[playing_level][0][i] == '\0') {
      fitted_map_width = i;
      break;
    }
  for (int i = 0; i < SIZE; i++)
    if (maps[playing_level][i][0] == '\0') {
      fitted_map_height = i;
      break;
    }

  left_box_cnt = 0;
  undo_count = 0;
  remain_undo_cnt = 5;  // 레벨 시작/재시작 시 undo 기회 5번으로 초기화
  op = ' ';
  is_complete_level = false;

  copy_map(playing_map, maps[playing_level], fitted_map_height,
           fitted_map_width);
  for (int i = 0; i < fitted_map_height; i++) {
    for (int j = 0; j < fitted_map_width; j++) {
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
  while (1) {
    system("clear");
    if (is_showing_help) {
      show_help();
    } else {
      show_playing_map(name, playing_level + 1, fitted_map_height,
                       fitted_map_width, playing_map);
    }

    printf("\n\n");

    // >>> 참고 메시지 >>>
    if (is_first_game) {
      is_first_game = false;
      printf("Welcome %s\n", name);
    } else if (is_gone_next_level) {
      is_gone_next_level = false;
      printf("You are in the level %d, now.\n", playing_level + 1);
    } else if (is_again) {
      is_again = false;
      printf("Again\n");
    } else if (is_new) {
      is_new = false;
      printf("Replay from level %d\n", playing_level + 1);
    } else if (is_undo) {
      is_undo = false;
      printf("Undid\n");
    } else {  // 참고메시지 때문에 UI가 위아래로 움직이는 문제 해결하기 위해
      printf("\n");
    }
    // <<< 참고 메시지 <<<

    // >>> Moves Command display >>>
    printf("(Moves) %04d\n", moves_cnt);
    printf("(Command) %c\n", op);
    // <<< Moves Command display <<<

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
        moves_cnt = 0;
        goto SET_PLAYING_MAP_BY_PLAYING_LEVEL;
      }  // <<< next level로 이동 <<<
      else {
        printf("Good bye!!\n");
        return 0;
      }
    }
    // <<< Level Clear 한 경우 <<<

    // >>> user 입력 >>>
    op = tolower(getch());
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
      case 'u':
        is_undo = true;
        break;
      case 'd':
        is_showing_help = true;
        break;
      case '\n':
        is_showing_help = false;
        break;
      case 'x':
        printf("Good bye\n");
        return 0;
        break;
      case 'a':
        is_again = true;
        break;
      case 'n':
        is_new = true;
        break;
    }
    // <<< user 입력 <<<

    if (dx != 0 || dy != 0) {  // >>> 이동 조작키를 눌렀을 때 >>>
      // (이동 관련은 if문 안에서만 처리하기)
      int ny = player_y + dy;
      int nx = player_x + dx;
      if (playing_map[ny][nx] == '#') continue;

      bool is_pushing_box = false;
      int box_ny, box_nx;

      if (playing_map[ny][nx] == '$') {
        box_ny = ny + dy;
        box_nx = nx + dx;
        char next_block = playing_map[box_ny][box_nx];
        if (next_block == '#' || next_block == '$')
          continue;  // early return 박스 이동 불가한 경우 바로 다시 키 입력받기

        is_pushing_box = true;
      }

      // >>> 이동 전 상태 Undo 배열에 저장 >>>
      if (undo_count == 5) {
        for (int k = 0; k < 4; k++) {
          copy_map(history_map[k], history_map[k + 1], fitted_map_height,
                   fitted_map_width);
          history_player_y[k] = history_player_y[k + 1];
          history_player_x[k] = history_player_x[k + 1];
          history_left_box_cnt[k] = history_left_box_cnt[k + 1];
        }
        undo_count = 4;
      }

      copy_map(history_map[undo_count], playing_map, fitted_map_height,
               fitted_map_width);
      history_player_y[undo_count] = player_y;
      history_player_x[undo_count] = player_x;
      history_left_box_cnt[undo_count] = left_box_cnt;
      undo_count++;
      // <<< 이동 전 상태 Undo 배열에 저장 <<<

      // >>> 박스 및 플레이어 이동 처리 >>>
      if (is_pushing_box) {
        if (box_dest_map[box_ny][box_nx] != box_dest_map[ny][nx]) {
          if (box_dest_map[box_ny][box_nx])
            left_box_cnt--;
          else
            left_box_cnt++;
        }
        playing_map[box_ny][box_nx] = '$';
      }
      playing_map[ny][nx] = '@';
      playing_map[player_y][player_x] =
          (box_dest_map[player_y][player_x]) ? 'O' : ' ';  // 이동 전 user 위치
      player_y = ny;
      player_x = nx;
      moves_cnt++;

      if (left_box_cnt == 0)
        is_complete_level = true;  // Level Clear는 while 문 다시 시작할 때 처리
      // <<< 박스 및 플레이어 이동 처리 <<<
    }  // <<< 이동 조작키를 눌렀을 때 <<<
    else if (is_again) {
      goto SET_PLAYING_MAP_BY_PLAYING_LEVEL;
    } else if (is_new) {
      playing_level = 0;
      moves_cnt = 0;
      goto SET_PLAYING_MAP_BY_PLAYING_LEVEL;
    } else if (is_undo) {
      if (remain_undo_cnt > 0 && undo_count > 0) {
        undo_count--;

        copy_map(playing_map, history_map[undo_count], fitted_map_height,
                 fitted_map_width);
        player_y = history_player_y[undo_count];
        player_x = history_player_x[undo_count];
        left_box_cnt = history_left_box_cnt[undo_count];

        moves_cnt++;
        remain_undo_cnt--;
      } else {
        is_undo = false;  // 실행 불가이므로 다음 턴에서 Undid 메시지 미출력
      }
    }
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

void show_playing_map(char name[], int level, int height, int width,
                      char map[SIZE][SIZE]) {
  printf("================\n");
  printf(" %s in Level %d \n", name, level);
  printf("================\n");

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }
}

void show_help() {
  printf("=======================================\n");
  printf("        S O K O B A N   H E L P        \n");
  printf("=======================================\n");
  printf("h(왼쪽), j(아래), k(위), l(오른쪽)\n");
  printf("u(undo)\n");
  printf("a(again)\n");
  printf("n(new)\n");
  printf("x(exit)\n");
  printf("d(display help)\n");
  printf("enter(redraw map)\n");
}

void copy_map(char dest[SIZE][SIZE], char src[SIZE][SIZE], int height,
              int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      dest[i][j] = src[i][j];
    }
  }
}