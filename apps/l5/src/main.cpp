#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <gsl/gsl>
#include <gsl/pointers>
#include <gsl/span>
#include <memory>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// SDL_Window and SDL_Surface are opaque types — std::default_delete can't call
// delete on an incomplete type, so we route destruction through SDL's own
// functions.
struct SdlWindowDeleter {
  void operator()(SDL_Window *window) const noexcept {
    SDL_DestroyWindow(window);
  }
};
struct SdlSurfaceDeleter {
  void operator()(SDL_Surface *surface) const noexcept {
    SDL_DestroySurface(surface);
  }
};

using SdlWindowPtr = std::unique_ptr<SDL_Window, SdlWindowDeleter>;
using SdlSurfacePtr = std::unique_ptr<SDL_Surface, SdlSurfaceDeleter>;

namespace {

// Plain message — no format args, safe to pass through %s.
void Log(const char *msg) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
  SDL_Log("%s", msg);
}

// Format string — requires at least one argument so fmt is never a lone
// runtime string passed as the format.
template <typename Arg, typename... Args>
void Log(const char *fmt, Arg arg, Args... args) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
  SDL_Log(fmt, arg, args...);
}

} // namespace

class MySimpleGame {
public:
  MySimpleGame() = default;
  MySimpleGame(const MySimpleGame &) = delete;
  MySimpleGame(MySimpleGame &&) = delete;
  MySimpleGame &operator=(const MySimpleGame &) = delete;
  MySimpleGame &operator=(MySimpleGame &&) = delete;
  ~MySimpleGame() { SDL_Quit(); }

  bool Init() {
    bool success{true};
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      Log("SDL could not init. SDL error: %s\n", SDL_GetError());
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

  bool LoadMedia() {
    auto LoadSurface = [](const char *const image_path,
                          SdlSurfacePtr &surface) -> bool {
      if (surface.reset(SDL_LoadBMP(image_path)); !surface) {
        Log("Can't load image %s. Error: %s\n", image_path, SDL_GetError());
        return false;
      }
      return true;
    };

    return LoadSurface("assets/hello.bmp", hello_world_) &&
           LoadSurface("assets/ball.bmp", ball_);
  }

  void Logic() {
    EventLoop();
    Render();
#ifdef __EMSCRIPTEN__
    if (!running_)
      emscripten_cancel_main_loop();
#endif
  }

  [[nodiscard]] bool is_running() const { return running_; }

private:
  void EventLoop() {
    // Events
    SDL_Event event;
    SDL_zero(event);
    while (SDL_PollEvent(&event)) {
      if ((event.type == SDL_EVENT_KEY_UP && event.key.key == SDLK_ESCAPE) ||
          (event.type == SDL_EVENT_QUIT)) {
        running_ = false;
      }
    }
    const gsl::span<const bool, SDL_SCANCODE_COUNT> kKeyStates{
        SDL_GetKeyboardState(nullptr), SDL_SCANCODE_COUNT};
    if (kKeyStates[SDL_SCANCODE_DOWN] || kKeyStates[SDL_SCANCODE_S]) {
      ball_rect_.y += 1;
    }
    if (kKeyStates[SDL_SCANCODE_UP] || kKeyStates[SDL_SCANCODE_W]) {
      ball_rect_.y -= 1;
    }
    if (kKeyStates[SDL_SCANCODE_LEFT] || kKeyStates[SDL_SCANCODE_A]) {
      ball_rect_.x -= 1;
    }
    if (kKeyStates[SDL_SCANCODE_RIGHT] || kKeyStates[SDL_SCANCODE_D]) {
      ball_rect_.x += 1;
    }
  }

  void Render() {
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

  static constexpr int kWindowWidth{1920 / 2};
  static constexpr int kWindowHeight{1080 / 2};
  static constexpr int kMaxRgb8{0xFF};

  // Owned resources.
  SdlWindowPtr window_{nullptr};
  SdlSurfacePtr hello_world_{nullptr};
  SdlSurfacePtr ball_{nullptr};

  // State.
  bool running_{true};
  static constexpr int kBallSize = 50; // pix square.
  SDL_Rect ball_rect_{0, 0, kBallSize, kBallSize};
};

int main(int /*argc*/, char * /*args*/[]) {
  int exitCode{0};

  MySimpleGame game;

  if (!game.Init()) {
    Log("Failed to init\n");
    exitCode = 1;
  } else {
    if (!game.LoadMedia()) {
      Log("Failed to load media\n");
      exitCode = 2;
    } else {

#ifdef __EMSCRIPTEN__
      emscripten_set_main_loop_arg(
          [](void *arg) { static_cast<MySimpleGame *>(arg)->Logic(); }, &game,
          0, 1);
#else
      while (game.is_running()) {
        game.Logic();
        SDL_Delay(1); // native only; in the browser rAF handles pacing
      }
#endif
    }
  }
  return exitCode;
}
