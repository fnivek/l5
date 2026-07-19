// Project Includes.
#include "l5/game_manager.h"
#include "l5/sdl_wrappers.h"

// 3rd Party.
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <gsl/pointers>
#include <gsl/span>
#include <gsl/util>

// Std.
#include <cmath>

GameManager::~GameManager() { SDL_Quit(); }

bool GameManager::Init() {
  bool success{true};
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SdlWrap::Log("SDL could not init. SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    window_.reset(
        SDL_CreateWindow("Tutorial land.", kWindowWidth, kWindowHeight, 0));
    if (!window_) {
      success = false;
    }
  }
  return success;
}

bool GameManager::LoadMedia() {
  auto LoadSurface = [](const char *const image_path,
                        SdlWrap::SurfacePtr &surface) -> bool {
    if (surface.reset(SDL_LoadBMP(image_path)); !surface) {
      SdlWrap::Log("Can't load image %s. Error: %s\n", image_path,
                   SDL_GetError());
      return false;
    }
    return true;
  };

  return LoadSurface("assets/hello.bmp", hello_world_) &&
         LoadSurface("assets/ball.bmp", ball_);
}

void GameManager::Logic() {
  EventLoop();
  Render();
}

void GameManager::EventLoop() {
  // Events
  SDL_Event event;
  SDL_zero(event);
  while (SDL_PollEvent(&event)) {
    if ((event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE) ||
        (event.type == SDL_EVENT_QUIT)) {
      running_ = false;
    }
  }

  SDL_Point dir{0, 0};
  const gsl::span<const bool, SDL_SCANCODE_COUNT> kKeyStates{
      SDL_GetKeyboardState(nullptr), SDL_SCANCODE_COUNT};
  if (kKeyStates[SDL_SCANCODE_DOWN] || kKeyStates[SDL_SCANCODE_S]) {
    dir.y = 1;
  }
  if (kKeyStates[SDL_SCANCODE_UP] || kKeyStates[SDL_SCANCODE_W]) {
    dir.y = -1;
  }
  if (kKeyStates[SDL_SCANCODE_LEFT] || kKeyStates[SDL_SCANCODE_A]) {
    dir.x = -1;
  }
  if (kKeyStates[SDL_SCANCODE_RIGHT] || kKeyStates[SDL_SCANCODE_D]) {
    dir.x = 1;
  }
  // Manage ball motion dynamics.
  const Uint64 now = SDL_GetTicksNS();
  constexpr double kNsPerSec = 1e9;
  const double dt = gsl::narrow_cast<double>(now - last_time_) / kNsPerSec;
  last_time_ = now;

  if (dir.x == 0 && dir.y == 0) {
    // Decelerate toward zero; stop exactly rather than oscillating.
    const double brake = kBrake * dt;
    velx_ =
        (std::abs(velx_) <= brake) ? 0.0 : velx_ - std::copysign(brake, velx_);
    vely_ =
        (std::abs(vely_) <= brake) ? 0.0 : vely_ - std::copysign(brake, vely_);
  } else {
    velx_ += kAccel * dir.x * dt;
    vely_ += kAccel * dir.y * dt;
  }
  posx_ += velx_ * dt;
  posy_ += vely_ * dt;

  // Wrap the pose to the edge of the screen.
  const auto x_shift = kWindowWidth + ball_rect_.w;
  if (posx_ < -ball_rect_.w) {
    posx_ += x_shift;
  } else if (posx_ > kWindowWidth) {
    posx_ -= x_shift;
  }
  const auto y_shift = kWindowHeight + ball_rect_.h;
  if (posy_ < -ball_rect_.h) {
    posy_ += y_shift;
  } else if (posy_ > kWindowHeight) {
    posy_ -= y_shift;
  }

  // Round to the nearest integer.
  ball_rect_.x = gsl::narrow_cast<int>(posx_);
  ball_rect_.y = gsl::narrow_cast<int>(posy_);
}

void GameManager::Render() {
  const gsl::not_null<SDL_Surface *> screen_surface =
      SDL_GetWindowSurface(window_.get());
  // Draw
  SDL_FillSurfaceRect(
      screen_surface, nullptr,
      SDL_MapSurfaceRGB(screen_surface, kMaxRgb8, kMaxRgb8, kMaxRgb8));
  SDL_BlitSurface(hello_world_.get(), nullptr, screen_surface, nullptr);
  SDL_BlitSurface(ball_.get(), nullptr, screen_surface, &ball_rect_);
  SDL_UpdateWindowSurface(window_.get());
}
