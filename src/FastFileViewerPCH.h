#pragma once

#include "util/Assert.h"
#include "util/Log.h"
#include "util/Types.h"
#include "util/Util.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#pragma warning(push, 0)
#include <vulkan/vulkan.h>
#pragma warning(pop)

#if defined(FFV_WINDOWS)
    #include <Windows.h>
#endif
