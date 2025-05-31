// 引入必要的標頭檔
// emscripten.h 用於 WebAssembly 編譯
// stdio.h 用於標準輸入輸出
// stdlib.h 用於隨機數生成
// string.h 用於字串操作
// time.h 用於時間相關功能
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

// 定義遊戲區域大小
// WIDTH 和 HEIGHT 定義遊戲區域的寬度和高度
// MAX_SNAKE_LENGTH 定義蛇的最大可能長度
#define WIDTH 20
#define HEIGHT 20
#define MAX_SNAKE_LENGTH (WIDTH * HEIGHT)

// 定義點的結構，用於表示蛇身和食物位置
// x 和 y 分別表示在遊戲區域中的橫縱座標
typedef struct {
  int x, y;
} Point;

/*
 * 方向定義：
 * 0 向上 - 蛇頭向上移動
 * 1 向右 - 蛇頭向右移動
 * 2 向下 - 蛇頭向下移動
 * 3 向左 - 蛇頭向左移動
 */
Point snake[MAX_SNAKE_LENGTH]; // 蛇的身體，使用陣列儲存每個節點的位置
Point food;                    // 食物位置
int snake_length = 1;          // 蛇的當前長度，初始值為1
int direction = 1;             // 移動方向，初始值為向右
int game_over = 0;             // 遊戲結束標誌，0表示遊戲進行中，1表示遊戲結束
int score = 0;                 // 分數，吃到食物時增加

// 初始化遊戲
// 設定蛇的初始位置和第一個食物
void init_game() {
  // 將蛇的初始位置設在畫面中央
  snake[0].x = WIDTH / 2;
  snake[0].y = HEIGHT / 2;
  // 隨機生成食物位置
  food.x = rand() % WIDTH;
  food.y = rand() % HEIGHT;
}
// 更新遊戲狀態、處理蛇的移動、碰撞檢測和食物收集
void update_game() {
  // 保存蛇尾位置，用於吃到食物時增長
  Point tail = snake[snake_length - 1];

  // 移動蛇身：從尾部開始，每個節點移動到前一個節點的位置
  for (int i = snake_length - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // 根據方向移動蛇頭
  switch (direction) {
  case 0: // 向上移動
    snake[0].y--;
    break;
  case 1: // 向右移動
    snake[0].x++;
    break;
  case 2: // 向下移動
    snake[0].y++;
    break;
  case 3: // 向左移動
    snake[0].x--;
    break;
  }

  // 處理穿牆：當蛇碰到邊界時，從另一側出現
  if (snake[0].x < 0)
    snake[0].x = WIDTH - 1;
  if (snake[0].x >= WIDTH)
    snake[0].x = 0;
  if (snake[0].y < 0)
    snake[0].y = HEIGHT - 1;
  if (snake[0].y >= HEIGHT)
    snake[0].y = 0;

  // 檢查是否撞到自己：遍歷蛇身，檢查是否與蛇頭位置重疊
  for (int i = 1; i < snake_length; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      game_over = 1;
      return;
    }
  }

  // 檢查是否吃到食物
  if (snake[0].x == food.x && snake[0].y == food.y) {
    snake_length++; // 增加蛇的長度
    score += 10;    // 增加分數
    // 生成新的食物位置，確保不會出現在蛇身上
    do {
      food.x = rand() % WIDTH;
      food.y = rand() % HEIGHT;
      int on_snake = 0;
      // 檢查新食物位置是否與蛇身重疊
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
    // 在蛇尾添加新的節點
    snake[snake_length - 1] = tail;
  }
}

// 遊戲主循環
// 持續更新遊戲狀態，直到遊戲結束
void game_loop() {
  if (!game_over) {
    update_game();
  } else {
    emscripten_cancel_main_loop(); // 遊戲結束時停止主循環
  }
}

// 處理鍵盤輸入
// 根據按鍵改變蛇的移動方向
EMSCRIPTEN_KEEPALIVE void handle_key(int key) {
  switch (key) {
  case 38:              // 上箭頭
    if (direction != 2) // 防止直接反向移動
      direction = 0;
    break;
  case 39: // 右箭頭
    if (direction != 3)
      direction = 1;
    break;
  case 40: // 下箭頭
    if (direction != 0)
      direction = 2;
    break;
  case 37: // 左箭頭
    if (direction != 1)
      direction = 3;
    break;
  case 27: // ESC 鍵
    game_over = 1;
    break;
  case 81: // Q 鍵
    game_over = 1;
    break;
  }
}

// 以下函數用於與 JavaScript 互動
// 這些函數被標記為 EMSCRIPTEN_KEEPALIVE 以確保在編譯時保留
EMSCRIPTEN_KEEPALIVE int get_snake_length() { return snake_length; }
EMSCRIPTEN_KEEPALIVE int get_snake_x(int i) { return snake[i].x; }
EMSCRIPTEN_KEEPALIVE int get_snake_y(int i) { return snake[i].y; }
EMSCRIPTEN_KEEPALIVE int get_food_x() { return food.x; }
EMSCRIPTEN_KEEPALIVE int get_food_y() { return food.y; }
EMSCRIPTEN_KEEPALIVE int get_score() { return score; }
EMSCRIPTEN_KEEPALIVE int is_game_over() { return game_over; }

// 初始化隨機數生成器並啟動遊戲
int main() {
  srand(time(NULL));                         // 初始化隨機數生成器
  init_game();                               // 初始化遊戲
  emscripten_set_main_loop(game_loop, 7, 1); // 設置遊戲主循環，7 FPS
  return 0;
}
