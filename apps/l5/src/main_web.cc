#include <emscripten/emscripten.h>

#include "l5/game_manager.h"
#include "l5/sdl_wrappers.h"

int main(int /*argc*/, char * /*args*/[]) {
  int exitCode{0};

  GameManager gm;

  if (!gm.Init()) {
    SdlWrap::Log("Failed to init\n");
    exitCode = 1;
  } else {
    if (!gm.LoadMedia()) {
      SdlWrap::Log("Failed to load media\n");
      exitCode = 2;
    } else {
      emscripten_set_main_loop_arg(
          [](void *arg) {
            auto *gm_ptr = static_cast<GameManager *>(arg);
            gm_ptr->Logic();
            if (!gm_ptr->is_running())
              emscripten_cancel_main_loop();
          },
          &gm, 0, 1);
    }
  }
  return exitCode;
}
