#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "felix86/common/state.hpp"
#include "felix86/common/types.hpp"
#include "felix86/emulator.hpp"

/*
    Loads tests from the FEX test suite at external/FEX/ASM
    Thanks to FEX for providing the test suite:
    https://github.com/FEX-Emu/FEX
*/

struct FEXTestLoader {
    FEXTestLoader(const std::filesystem::path& path);
    ~FEXTestLoader();

    void Run();

    void Validate();

    static void RunTest(const std::filesystem::path& path);

private:
    TestConfig config{};
    std::vector<u8> buffer{};
    std::string json{};
    std::array<std::optional<u64>, 16> expected_gpr{};
    std::array<std::optional<XmmReg>, 16> expected_xmm{};
    std::array<std::optional<u64>, 8> expected_mm{};
    std::vector<std::pair<u64, u64>> memory_mappings{};
    std::vector<std::pair<void*, u64>> munmap_me;
};