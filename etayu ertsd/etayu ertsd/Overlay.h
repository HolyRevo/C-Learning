#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <mutex>
#include <atomic>
#include "Memory.h"

class Overlay {
public:
    Overlay();
    ~Overlay();
    bool Create();
    void Run();
    void Shutdown();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void Render();

    HWND hwnd = nullptr;
    ULONG_PTR gdiplusToken = 0;
    std::atomic<bool> running{ true };

    Memory memory;
    std::vector<PlayerBox> playerBoxes;
    std::mutex boxMutex;

    // Memory reading thread
    void MemoryThread();
};