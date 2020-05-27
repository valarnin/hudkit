#include <keybinder-3.0/keybinder.h>
#include <stdbool.h>

bool lockState = true;

void hotkey_handler(const char *keystring, void *data) {
  if (lockState) {
    enable_move_resize();
  } else {
    disable_move_resize();
  }
  lockState = !lockState;
}

void register_hotkey() {
  keybinder_init();
  keybinder_bind(hotkey, hotkey_handler, NULL);
}

void unregister_hotkey() { keybinder_unbind(hotkey, hotkey_handler); }
