#ifndef L5_SDL_WRAPPERS_H_
#define L5_SDL_WRAPPERS_H_

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <memory>

namespace SdlWrap {
// SDL_Window and SDL_Surface are opaque types — std::default_delete can't call
// delete on an incomplete type, so we route destruction through SDL's own
// functions.
struct WindowDeleter {
  void operator()(SDL_Window *window) const noexcept {
    SDL_DestroyWindow(window);
  }
};
struct SurfaceDeleter {
  void operator()(SDL_Surface *surface) const noexcept {
    SDL_DestroySurface(surface);
  }
};

using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

// Plain message — no format args, safe to pass through %s.
inline void Log(const char *msg) noexcept {
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
} // namespace SdlWrap

#endif // L5_SDL_WRAPPERS_H_
