#ifndef L5_GAME_MANAGER_H_
#define L5_GAME_MANAGER_H_

#include "l5/sdl_wrappers.h"
#include <SDL3/SDL_stdinc.h>

/** Entry point for L5.
 *
 */
class GameManager {
public:
  GameManager() = default;
  GameManager(const GameManager &) = delete;
  GameManager(GameManager &&) = delete;
  GameManager &operator=(const GameManager &) = delete;
  GameManager &operator=(GameManager &&) = delete;
  ~GameManager();

  bool Init();

  bool LoadMedia();

  void Logic();

  [[nodiscard]] bool is_running() const { return running_; }

private:
  void EventLoop();

  void Render();

  static constexpr int kWindowWidth{1920 / 2};
  static constexpr int kWindowHeight{1080 / 2};
  static constexpr int kMaxRgb8{0xFF};

  // Owned resources.
  SdlWrap::WindowPtr window_{nullptr};
  SdlWrap::SurfacePtr hello_world_{nullptr};
  SdlWrap::SurfacePtr ball_{nullptr};

  // State.
  bool running_{true};
  static constexpr int kBallSize = 50; // pix square.
  SDL_Rect ball_rect_{0, 0, kBallSize, kBallSize};
  // Step time.
  Uint64 last_time_{0};
  // Acceleration of the ball in px/s.
  static constexpr double kAccel{500};
  static constexpr double kBrake{1000};
  // x y Velocity in px/s.
  double velx_{0};
  double vely_{0};
  // x y Position in px.
  double posx_{0};
  double posy_{0};
};

#endif // L5_GAME_MANAGER_H_
