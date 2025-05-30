#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

#define WIDTH 20
#define HEIGHT 20
#define MAX_SNAKE_LENGTH (WIDTH * HEIGHT)

typedef struct {
  int x, y;
} Point;

/*
 * 0 up
 * 1 right
 * 2 down
 * 3 left
 */
Point snake[MAX_SNAKE_LENGTH];
Point food;
int snake_length = 1;
int direction = 1;
int game_over = 0;
int score = 0;

void init_game() {
  snake[0].x = WIDTH / 2;
  snake[0].y = HEIGHT / 2;
  food.x = rand() % WIDTH;
  food.y = rand() % HEIGHT;
}

void update_game() {
  Point tail = snake[snake_length - 1];

  for (int i = snake_length - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  switch (direction) {
  case 0:
    snake[0].y--;
    break;
  case 1:
    snake[0].x++;
    break;
  case 2:
    snake[0].y++;
    break;
  case 3:
    snake[0].x--;
    break;
  }

  if (snake[0].x < 0)
    snake[0].x = WIDTH - 1;
  if (snake[0].x >= WIDTH)
    snake[0].x = 0;
  if (snake[0].y < 0)
    snake[0].y = HEIGHT - 1;
  if (snake[0].y >= HEIGHT)
    snake[0].y = 0;

  for (int i = 1; i < snake_length; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      game_over = 1;
      return;
    }
  }

  if (snake[0].x == food.x && snake[0].y == food.y) {
    snake_length++;
    score += 10;
    do {
      food.x = rand() % WIDTH;
      food.y = rand() % HEIGHT;
      int on_snake = 0;
      for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == food.x && snake[i].y == food.y) {
          on_snake = 1;
          break;
        }
      }
      if (!on_snake) {
        break;
      }
    } while (1);
    snake[snake_length - 1] = tail;
  }
}

void game_loop() {
  if (!game_over) {
    update_game();
  } else {
    emscripten_cancel_main_loop();
  }
}

EMSCRIPTEN_KEEPALIVE void handle_key(int key) {
  switch (key) {
  case 38: // up
    if (direction != 2)
      direction = 0;
    break;
  case 39: // right
    if (direction != 3)
      direction = 1;
    break;
  case 40: // down
    if (direction != 0)
      direction = 2;
    break;
  case 37: // left
    if (direction != 1)
      direction = 3;
    break;
  case 27: // escape
    game_over = 1;
    break;
  case 81: // q
    game_over = 1;
    break;
  }
}

EMSCRIPTEN_KEEPALIVE int get_snake_length() { return snake_length; }
EMSCRIPTEN_KEEPALIVE int get_snake_x(int i) { return snake[i].x; }
EMSCRIPTEN_KEEPALIVE int get_snake_y(int i) { return snake[i].y; }
EMSCRIPTEN_KEEPALIVE int get_food_x() { return food.x; }
EMSCRIPTEN_KEEPALIVE int get_food_y() { return food.y; }
EMSCRIPTEN_KEEPALIVE int get_score() { return score; }
EMSCRIPTEN_KEEPALIVE int is_game_over() { return game_over; }

int main() {
  srand(time(NULL));
  init_game();
  emscripten_set_main_loop(game_loop, 7, 1);
  return 0;
}
