
#pragma once

// stdc++
#include <filesystem>
#include <string>
#include <vector>

// application
#include "config.hpp"

/**************************************************************************************************/

namespace hyde {

/**************************************************************************************************/

std::vector<std::filesystem::path> autodetect_toolchain_paths();

std::filesystem::path autodetect_resource_directory();

#if HYDE_PLATFORM(APPLE)
std::filesystem::path autodetect_sysroot_directory();
#endif // HYDE_PLATFORM(APPLE)

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
