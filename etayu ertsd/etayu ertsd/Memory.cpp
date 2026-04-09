#include "Memory.h"
#include "Offsets.h"
#include "Logger.h"
#include <tlhelp32.h>
#include <psapi.h>

Memory::Memory() = default;

Memory::~Memory() { Detach(); }

bool Memory::FindProcess() {
    PROCESSENTRY32 entry{ sizeof(PROCESSENTRY32) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    if (Process32First(snapshot, &entry)) {
        do {
            if (_stricmp(entry.szExeFile, "cs2.exe") == 0) {
                hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, entry.th32ProcessID);
                found = true;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found && hProcess != nullptr;
}

bool Memory::FindClientModule() {
    HMODULE modules[1024];
    DWORD needed;
    if (EnumProcessModules(hProcess, modules, sizeof(modules), &needed)) {
        for (DWORD i = 0; i < (needed / sizeof(HMODULE)); i++) {
            char moduleName[MAX_PATH];
            if (GetModuleBaseNameA(hProcess, modules[i], moduleName, sizeof(moduleName))) {
                if (_stricmp(moduleName, "client.dll") == 0) {
                    clientBase = (uint64_t)modules[i];
                    return true;
                }
            }
        }
    }
    return false;
}

bool Memory::Attach() {
    if (IsAttached()) return true;
    if (!FindProcess()) return false;
    if (!FindClientModule()) {
        Detach();
        return false;
    }
    Logger::Log("Attached to cs2.exe | client.dll base: 0x" + std::to_string(clientBase));
    return true;
}

void Memory::Detach() {
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = nullptr;
        clientBase = 0;
    }
}

template<typename T>
T Memory::Read(uint64_t address) const {
    T value{};
    if (!hProcess || address == 0) return value;
    ReadProcessMemory(hProcess, (LPCVOID)address, &value, sizeof(T), nullptr);
    return value;
}

void Memory::ReadArray(uint64_t address, void* buffer, size_t size) const {
    if (!hProcess || address == 0) return;
    ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, nullptr);
}

std::vector<PlayerBox> Memory::GetPlayerBoxes() {
    std::vector<PlayerBox> boxes;

    if (!IsAttached() || !Attach()) return boxes;

    uint64_t localPlayer = Read<uint64_t>(clientBase + offsets::dwLocalPlayer);
    if (!localPlayer) return boxes;

    uint64_t entityList = Read<uint64_t>(clientBase + offsets::dwEntityList);
    if (!entityList) return boxes;

    float viewMatrix[4][4]{};
    ReadArray(clientBase + offsets::dwViewMatrix, viewMatrix, sizeof(viewMatrix));

    // Simple 64-player loop (common CS2 pattern - update stride/offset if game changes)
    for (int i = 0; i < 64; ++i) {
        uint64_t entity = Read<uint64_t>(entityList + i * 0x78); // <-- common stride, update if needed
        if (!entity || entity == localPlayer) continue;

        uint64_t pawn = Read<uint64_t>(entity + 0x10); // adjust if your offsets differ
        if (!pawn) continue;

        Vector3 origin = Read<Vector3>(pawn + offsets::m_vecOrigin);

        // Simple head offset (good enough for clean box)
        Vector3 headPos = origin;
        headPos.z += 72.0f; // player height head adjustment

        Vector2 screenHead, screenFeet;
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        if (WorldToScreen::Project(headPos, screenHead, viewMatrix, screenW, screenH) &&
            WorldToScreen::Project(origin, screenFeet, viewMatrix, screenW, screenH)) {

            PlayerBox box;
            box.head = screenHead;
            box.feet = screenFeet;
            box.valid = true;
            boxes.push_back(box);
        }
    }
    return boxes;
}