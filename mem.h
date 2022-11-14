#pragma once

#include <wtypes.h>
#include <cstring>
#include <vector>

namespace Memorie
{
	inline void overwrite(BYTE* dst, BYTE* src, unsigned int size);

	uintptr_t findAddr(uintptr_t ptr, std::vector<unsigned int> offsets);
};

