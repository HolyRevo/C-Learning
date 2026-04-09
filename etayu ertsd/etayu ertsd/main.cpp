#include "Overlay.h"
#include "Logger.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Logger::Log("=== CS2 Red Box ESP started ===");

    Overlay overlay;
    overlay.Run();

    Logger::Log("=== CS2 Red Box ESP exited ===");
    return 0;
}