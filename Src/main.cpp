#include <SDl2/SDL.h>
#include <array>
#include <iostream>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

typedef std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH> GameBoard;

namespace Snake {
  constexpr int max_size = BOARD_WIDTH*BOARD_HEIGHT;
  int score = 0;
  std::array<SDL_Point, max_size> Positions;
  int current_size = 0;
  SDL_Point fruit = {BOARD_WIDTH-1, BOARD_HEIGHT-1};
  void AddPosition(const SDL_Point& new_position) {
  Positions[current_size++] = new_position;
  }

  void NewFruitLocation() {
  int place = rand() % max_size;
  int offset = rand() % 2 * 2 - 1;
repeat:
  int x = place % BOARD_WIDTH;
  int y = (place - x) / BOARD_HEIGHT;
  for (int i = 0; i < current_size; i++) if (Positions[i].x == x && Positions[i].y == y) {
    place += offset;
    if (place < 0) place = max_size-1;
    else if (place >= max_size) place = 0;
    goto repeat;
  }
  fruit = {x, y};
  score += 10;
  }

  void UpdatePosition(const SDL_Point& new_position) {
    if (new_position.x == fruit.x && new_position.y == fruit.y) {
      AddPosition(new_position);
      NewFruitLocation();
      return;
    }
    for (int i = 0; i < current_size-1; i++) {
      Positions[i] = Positions[i+1]; 
    }
    Positions[current_size-1] = new_position;
  }
  bool ValidateNewPosition(const SDL_Point& new_position) {
   if (new_position.x < 0 || new_position.x >= BOARD_WIDTH || new_position.y < 0 || new_position.y >= BOARD_HEIGHT) return false;
   for (int i = 0; i < current_size-1; i++) if (Positions[i].x == new_position.x && Positions[i].y == new_position.y) return false;
   return true;
  }
  inline SDL_Point GetPosition() {
    return Positions[current_size-1];
  }
  
}

namespace Renderer {
  #define CELL_WIDTH 20
  #define SCALE 2
  bool running = true;
  bool won = false;
  SDL_Renderer * renderer;
  SDL_Window * window;
  SDL_Event event;
  const uint8_t * KeyboardState = SDL_GetKeyboardState(nullptr);
  void Init() {
    SDL_CreateWindowAndRenderer(CELL_WIDTH * BOARD_WIDTH * SCALE, CELL_WIDTH * BOARD_HEIGHT * SCALE, 0, &window, &renderer);
    SDL_SetWindowTitle( window, "Snake For MacOS");
    SDL_RenderSetLogicalSize(renderer, BOARD_WIDTH*CELL_WIDTH, BOARD_HEIGHT*CELL_WIDTH);
  }
  void SetTitle() {
    SDL_SetWindowTitle(window, std::string("Snake For MacOS : Score - " + std::to_string(Snake::score)).c_str());
  }
  void extracted() { return; }
  void Draw() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    const SDL_Rect fruit = {Snake::fruit.x * CELL_WIDTH, Snake::fruit.y * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH};
    SDL_RenderFillRect(renderer, &fruit);
    SDL_SetRenderDrawColor(renderer, 245, 151, 255, 255);
    for (int i = 0; i < Snake::current_size; i++) {
      const SDL_Rect body = {CELL_WIDTH*Snake::Positions[i].x, CELL_WIDTH*Snake::Positions[i].y, CELL_WIDTH, CELL_WIDTH};
      SDL_RenderFillRect(renderer, &body);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    for (int w = 0; w < BOARD_WIDTH; w++) for (int h = 0; h < BOARD_WIDTH; h++) {
      const SDL_Rect place = {CELL_WIDTH*w, CELL_WIDTH*h, CELL_WIDTH, CELL_WIDTH};
      SDL_RenderDrawRect(renderer, &place);
    }
    SDL_RenderPresent(renderer);
    if (Snake::current_size == Snake::max_size) {
      won = true;
      return; 
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
  }

  void Cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void Input() {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
      running = false;
      }
    }
  }
}

namespace Time {
  constexpr int MillisecondWait = 300;
  uint64_t elapsedTime = 0;
  uint64_t timer = 0;
  uint64_t deltaTime = 0;
  void CalcDeltaTime() {
  deltaTime = SDL_GetTicks64() - elapsedTime;
  elapsedTime = SDL_GetTicks64();
  }

}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  srand(time(NULL));
  Renderer::Init();
  Snake::AddPosition({0, 0});
  Snake::NewFruitLocation();
  SDL_Point SnakeDir = {0,1};
  while (Renderer::running) {
    Renderer::Input();
    if (Renderer::KeyboardState[SDL_SCANCODE_UP] || Renderer::KeyboardState[SDL_SCANCODE_W]) SnakeDir = {0, -1};
    if (Renderer::KeyboardState[SDL_SCANCODE_RIGHT]|| Renderer::KeyboardState[SDL_SCANCODE_D]) SnakeDir = {1, 0};
    if (Renderer::KeyboardState[SDL_SCANCODE_DOWN] || Renderer::KeyboardState[SDL_SCANCODE_S]) SnakeDir = {0 ,1};
    if (Renderer::KeyboardState[SDL_SCANCODE_LEFT] || Renderer::KeyboardState[SDL_SCANCODE_A]) SnakeDir = {-1, 0};
    if (Time::timer > Time::MillisecondWait) {
    Time::timer = 0;
    SDL_Point movement = {Snake::GetPosition().x + SnakeDir.x, Snake::GetPosition().y + SnakeDir.y};
    if (!Snake::ValidateNewPosition(movement)) Renderer::running = false;
    Snake::UpdatePosition(movement);
    Renderer::SetTitle();
    }
    Renderer::Draw();
    Time::CalcDeltaTime();
    Time::timer += Time::deltaTime;
  }
  goto cleanup;
win:
  Renderer::running = true;
  Renderer::Input();
  if (Renderer::running) goto win;
cleanup:
  Renderer::Cleanup();
  return 0;
}
