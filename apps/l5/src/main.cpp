#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
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

int *gFrameBuffer;
SDL_Window *gWindow{nullptr};
SDL_Surface *gScreenSurface{nullptr};
SDL_Surface *gHelloWorld{nullptr};

SDL_Renderer *gSDLRenderer;
SDL_Texture *gSDLTexture;
static int gDone;
constexpr int kWindowWidth{1920 / 2};
constexpr int kWindowHeight{1080 / 2};

auto init() -> bool;
auto loadMedia() -> bool;
auto close() -> void;

auto init() -> bool {
  bool success{true};
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("SDL could not init. SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (gWindow =
            SDL_CreateWindow("Tutorial land.", kWindowWidth, kWindowHeight, 0);
        gWindow == nullptr) {
      success = false;
    } else {
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }
  return success;
}

auto loadMedia() -> bool {
  bool success{true};

  std::string imagePath{"assets/hello.bmp"};
  if (gHelloWorld = SDL_LoadBMP(imagePath.c_str()); gHelloWorld == nullptr) {
    SDL_Log("Can't load image %s. Error: %s\n", imagePath.c_str(),
            SDL_GetError());
    success = false;
  }
  return success;
}

auto close() -> void {
  SDL_DestroySurface(gHelloWorld);
  gHelloWorld = nullptr;

  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;
  gScreenSurface = nullptr;

  SDL_Quit();
}

bool gRunning{true};
auto logic() -> void {
  // Events
  SDL_Event event;
  SDL_zero(event);
  while (SDL_PollEvent(&event) == true) {
    if (event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE) {
      gRunning = false;
    } else if (event.type == SDL_EVENT_QUIT) {
      gRunning = false;
    }
  }

  // Draw
  SDL_FillSurfaceRect(gScreenSurface, nullptr,
                      SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
  SDL_BlitSurface(gHelloWorld, nullptr, gScreenSurface, nullptr);
  SDL_UpdateWindowSurface(gWindow);

#ifdef __EMSCRIPTEN__
  if (!gRunning)
    emscripten_cancel_main_loop();
#endif
}

auto main(int argc, char *args[]) -> int {
  int exitCode{0};

  if (init() == false) {
    SDL_Log("Failed to init\n");
    exitCode = 1;
  } else {
    if (loadMedia() == false) {
      SDL_Log("Failed to load media\n");
      exitCode = 2;
    } else {

#ifdef __EMSCRIPTEN__
      emscripten_set_main_loop(logic, 0, 1);
#else
      while (gRunning) {
        logic();
        SDL_Delay(1); // native only; in the browser rAF handles pacing
      }
#endif
      bool quit = false;
    }
    close();
  }
  return exitCode;
}
