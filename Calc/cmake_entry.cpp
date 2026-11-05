/*
 * CMake-only entry-point bridge.
 *
 * gui.cpp keeps the platform entry points inside namespace GUI so that the
 * existing Visual Studio project and the CMake target can share the same
 * implementation without changing gui.cpp.
 */

#ifdef _WIN32

#include <windows.h>

namespace GUI {
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                   LPSTR command_line, int show_command) {
  return GUI::WinMain(instance, previous_instance, command_line, show_command);
}

#else

namespace GUI {
[[noreturn]] int main(int, char **) noexcept;
}

int main(int argc, char **argv) {
  return GUI::main(argc, argv);
}

#endif
