#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int *g_frame_buffer;
SDL_Window *g_window{nullptr};
SDL_Surface *g_screen_surface{nullptr};
SDL_Surface *g_hello_world{nullptr};
SDL_Surface *g_ball{nullptr};

constexpr int kWindowWidth{1920 / 2};
constexpr int kWindowHeight{1080 / 2};

SDL_Rect g_ball_rect{0, 0, 50, 50};

bool Init();
bool LoadMedia();
void Close();

bool Init() {
  bool success{true};
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("SDL could not init. SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (g_window =
            SDL_CreateWindow("Tutorial land.", kWindowWidth, kWindowHeight, 0);
        g_window == nullptr) {
      success = false;
    } else {
      g_screen_surface = SDL_GetWindowSurface(g_window);
    }
  }
  return success;
}

bool LoadMedia() {
  bool success{true};

  std::string image_path{"assets/hello.bmp"};
  if (g_hello_world = SDL_LoadBMP(image_path.c_str());
      g_hello_world == nullptr) {
    SDL_Log("Can't load image %s. Error: %s\n", image_path.c_str(),
            SDL_GetError());
    success = false;
  }

  image_path = "assets/ball.bmp";
  if (g_ball = SDL_LoadBMP(image_path.c_str()); g_ball == nullptr) {
    SDL_Log("Can't load image %s. Error: %s\n", image_path.c_str(),
            SDL_GetError());
    success = false;
  }

  return success;
}

void Close() {
  SDL_DestroySurface(g_hello_world);
  g_hello_world = nullptr;

  SDL_DestroyWindow(g_window);
  g_window = nullptr;
  g_screen_surface = nullptr;

  SDL_Quit();
}

bool g_running{true};
void Logic() {
  // Events
  SDL_Event event;
  SDL_zero(event);
  while (SDL_PollEvent(&event) == true) {
    if (event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE) {
      g_running = false;
    } else if (event.type == SDL_EVENT_QUIT) {
      g_running = false;
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      switch (event.key.key) {
      case SDLK_DOWN:
      case SDLK_S:
        break;
      case SDLK_UP:
      case SDLK_W:
        break;
      case SDLK_LEFT:
      case SDLK_A:
        break;
      case SDLK_RIGHT:
      case SDLK_D:
        break;
      default:
        // Do nothing.
        break;
      }
    }
  }
  const bool *const kKeyStates = SDL_GetKeyboardState(nullptr);
  if (kKeyStates[SDL_SCANCODE_DOWN] == true || kKeyStates[SDLK_S] == true) {
    g_ball_rect.y += 1;
  }
  if (kKeyStates[SDL_SCANCODE_UP] == true || kKeyStates[SDLK_W] == true) {
    g_ball_rect.y -= 1;
  }
  if (kKeyStates[SDL_SCANCODE_LEFT] == true || kKeyStates[SDLK_A] == true) {
    g_ball_rect.x -= 1;
  }
  if (kKeyStates[SDL_SCANCODE_RIGHT] == true || kKeyStates[SDLK_D] == true) {
    g_ball_rect.x += 1;
  }

  // Draw
  SDL_FillSurfaceRect(g_screen_surface, nullptr,
                      SDL_MapSurfaceRGB(g_screen_surface, 0xFF, 0xFF, 0xFF));
  SDL_BlitSurface(g_hello_world, nullptr, g_screen_surface, nullptr);
  SDL_BlitSurface(g_ball, nullptr, g_screen_surface, &g_ball_rect);
  SDL_UpdateWindowSurface(g_window);

#ifdef __EMSCRIPTEN__
  if (!g_running)
    emscripten_cancel_main_loop();
#endif
}

int main(int argc, char *args[]) {
  int exitCode{0};

  if (Init() == false) {
    SDL_Log("Failed to init\n");
    exitCode = 1;
  } else {
    if (LoadMedia() == false) {
      SDL_Log("Failed to load media\n");
      exitCode = 2;
    } else {

#ifdef __EMSCRIPTEN__
      emscripten_set_main_loop(Logic, 0, 1);
#else
      while (g_running) {
        Logic();
        SDL_Delay(1); // native only; in the browser rAF handles pacing
      }
#endif
      bool quit = false;
    }
    Close();
  }
  return exitCode;
}
