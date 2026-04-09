#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include "WorldToScreen.h"

struct PlayerBox {
    Vector2 head;
    Vector2 feet;
    bool valid = false;
};

class Memory {
public:
    Memory();
    ~Memory();
    bool Attach();
    void Detach();
    bool IsAttached() const { return hProcess != nullptr; }

    template<typename T>
    T Read(uint64_t address) const;

    void ReadArray(uint64_t address, void* buffer, size_t size) const;

    std::vector<PlayerBox> GetPlayerBoxes();

private:
    HANDLE hProcess = nullptr;
    uint64_t clientBase = 0;
    bool FindProcess();
    bool FindClientModule();
};