#pragma once

#include "felix86/common/types.hpp"

int ioctl32_fat(int fd, u32 cmd, u32 args);
