#include "Overlay.h"
#include "Logger.h"
#include "WorldToScreen.h"
#include <thread>

Overlay::Overlay() { Logger::Init(); }

Overlay::~Overlay() { Shutdown(); }

bool Overlay::Create() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"CS2RedBoxOverlay";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"CS2 Red Box Overlay",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, GetModuleHandle(nullptr), this
    );

    if (!hwnd) {
        Logger::Error("Failed to create overlay window");
        return false;
    }

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Start memory thread
    std::thread(&Overlay::MemoryThread, this).detach();

    Logger::Log("Overlay window created successfully");
    return true;
}

LRESULT CALLBACK Overlay::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // GDI+ drawing happens in Render()
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Overlay::MemoryThread() {
    while (running) {
        if (memory.IsAttached() || memory.Attach()) {
            auto newBoxes = memory.GetPlayerBoxes();
            {
                std::lock_guard<std::mutex> lock(boxMutex);
                playerBoxes = std::move(newBoxes);
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS update
    }
}

void Overlay::Render() {
    if (!hwnd) return;

    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);
    graphics.Clear(Gdiplus::Color(0, 0, 0)); // color key = transparent

    Gdiplus::Pen redPen(Gdiplus::Color(255, 0, 0), 2.5f);

    {
        std::lock_guard<std::mutex> lock(boxMutex);
        for (const auto& box : playerBoxes) {
            if (!box.valid) continue;
            float x = min(box.head.x, box.feet.x);
            float y = min(box.head.y, box.feet.y);
            float w = abs(box.head.x - box.feet.x);
            float h = abs(box.head.y - box.feet.y);

            graphics.DrawRectangle(&redPen, x, y, w, h);
        }
    }

    ReleaseDC(hwnd, hdc);
}

void Overlay::Run() {
    if (!Create()) return;

    MSG msg{};
    while (running && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Render every frame
        Render();

        // F12 exit
        if (GetAsyncKeyState(VK_F12) & 0x8000) {
            running = false;
        }
    }
}

void Overlay::Shutdown() {
    running = false;
    if (hwnd) DestroyWindow(hwnd);
    if (gdiplusToken) Gdiplus::GdiplusShutdown(gdiplusToken);
    Logger::Log("Overlay shut down cleanly");
}