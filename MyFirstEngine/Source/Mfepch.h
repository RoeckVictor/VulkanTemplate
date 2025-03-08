#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "MyFirstEngine/Log.h"
#include "MyFirstEngine/Debug/Instrumentor.h"

#ifdef MFE_PLATFORM_WINDOWS
	#include <Windows.h>
#endif