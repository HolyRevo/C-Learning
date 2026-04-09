#pragma once
#include <cstdint>

// =============================================
// THESE OFFSETS MUST BE UPDATED AFTER EVERY CS2 UPDATE
// Check reliable sources:
//   - https://www.unknowncheats.me/forum/counter-strike-2-a/
//   - Search GitHub for "CS2 offsets" (updated dumps)
//   - Or use Cheat Engine / ReClass to find them yourself
// =============================================
namespace offsets {
    constexpr std::uint64_t dwLocalPlayer = 0x0; // PLACEHOLDER - UPDATE
    constexpr std::uint64_t dwEntityList = 0x0; // PLACEHOLDER - UPDATE
    constexpr std::uint64_t dwViewMatrix = 0x0; // PLACEHOLDER - UPDATE

    // Player fields (used for origin + simple head offset)
    constexpr std::uint64_t m_vecOrigin = 0x0; // PLACEHOLDER - UPDATE (C_BaseEntity)
}